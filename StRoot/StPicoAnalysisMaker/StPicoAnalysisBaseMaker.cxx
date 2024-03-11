#define StPicoAnalysisBaseMaker_cxx

#include "StPicoAnalysisBaseMaker.h"
//ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
// STAR includes
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoTrack.h"
#include "StPicoEvent/StPicoBTowHit.h"
#include "StPicoEvent/StPicoBEmcPidTraits.h"
// centrality includes
#include "StRoot/StRefMultCorr/CentralityMaker.h"
#include "StRoot/StRefMultCorr/StRefMultCorr.h"
//MyAnalysisMaker includes
#include "StEmcPosition2.h"
// TStar includes
#include "StRoot/TStarEventClass/TStarEvent.h"
#include "StRoot/TStarEventClass/TStarArrays.h"

ClassImp(StPicoAnalysisBaseMaker);

using namespace std;

StPicoAnalysisBaseMaker::StPicoAnalysisBaseMaker (unsigned int imaker, string name, string output, bool dodebug) : 
StMaker(name.c_str()), 
iMaker(imaker),
thisMakerName(name), 
histoFileName(output),
doDebug(dodebug) {
    // default constructor
    if(doDebug){cout<<"StPicoAnalysisBaseMaker::StPicoAnalysisBaseMaker() - Starting"<<endl;
    cout<<"Doing TStarArrays::addArray(\"event\")"<<endl;}

    TStarArrays::addArray("event");

    if(doDebug) cout<<"StPicoAnalysisBaseMaker::StPicoAnalysisBaseMaker() - Done"<<endl;
}

StPicoAnalysisBaseMaker::~StPicoAnalysisBaseMaker(){
    if(efficiencyFile) delete efficiencyFile; 
}

void StPicoAnalysisBaseMaker::deleteHistograms(){
    for(auto& hist1D : histos1D){
        cout<<"Deleting "<<hist1D.first<<endl;
        if(hist1D.second) delete hist1D.second;
    }
    for(auto& hist2D : histos2D){
        cout<<"Deleting "<<hist2D.first<<endl;
        if(hist2D.second) delete hist2D.second;
    }
    histos1D.clear();
    histos2D.clear();
} 

Int_t StPicoAnalysisBaseMaker::Init(){
    cout<<"StPicoAnalysisBaseMaker::Init() - Starting"<<endl;
    picoDstMaker = dynamic_cast<StPicoDstMaker*>(GetMaker("picoDst"));
    assert(((void)"No PicoDstMaker!", picoDstMaker));
    if(doDebug)cout<<"Got picoDstMaker"<<endl;

    if(!doRunbyRun)setUpBadRuns(); //Probably deprecated, might add this into StRefMultCorr

    if(!doppAnalysis){
        //grefmultCorr = CentralityMaker::instance()->getgRefMultCorr_P18ih_VpdMB30_AllLumi();
        grefmultCorr = new StRefMultCorr(refmultCorrName.c_str());
        if(doDebug){
            cout<<"Set up grefmultCorr..."<<endl;
            grefmultCorr->print();
        }
    }

    if(efficiencyFileName != ""){
        efficiencyFile = new TFile(efficiencyFileName.c_str(), "READ");
        if(!efficiencyFile->IsOpen()){
            cout<<"Could not open efficiency file!"<<endl;
            return kStWarn;
        }if(doDebug)cout<<"Opened efficiency file: "<<efficiencyFileName<<endl;
    }

    declareStatHistos();

    for(auto& hist : histos1D){
        hist.second->Sumw2();
    }

    for(auto& hist : histos2D){
        hist.second->Sumw2();
    }

    if(doDebug)cout<<"StPicoAnalysisBaseMaker::Init() - Done"<<endl;
    return kStOK;
}

void StPicoAnalysisBaseMaker::writeHistograms(){
    if(histoFileName != ""){
        histOut = new TFile(histoFileName.c_str(), "UPDATE");
        histOut->cd();
        histOut->mkdir(GetName());
        histOut->cd(GetName());
        for(const auto& hist : histos1D){
            hist.second->Write();
            if(doDebug)cout<<"Wrote "<<hist.first<<endl;
        }
        for(const auto& hist : histos2D){
            hist.second->Write();
            if(doDebug)cout<<"Wrote "<<hist.first<<endl;
        }
        histOut->cd();
        histOut->Write();
        histOut->Close();
        if(doDebug)cout<<"Wrote histograms to "<<histoFileName<<endl;
    }
}

Int_t StPicoAnalysisBaseMaker::Finish(){
    cout<< "StPicoAnalysisBaseMaker::Finish()"<<endl;
    if(efficiencyFile)efficiencyFile->Close();
    writeHistograms(); 
    return kStOk;
}

bool StPicoAnalysisBaseMaker::isEventGood(){
    picoEvent = StPicoDst::event();
    if(!picoEvent){
        cout<<" No PicoEvent! Skip! " << endl;
        return false;
    }if(doEventDebug){cout<<"Got StPicoEvent!"<<endl;}fillHist1D("hEventStats", 0);

    //Reject bad runs here..., if doing run by run jobs, reject bad runs while submitting jobs
    runID = picoEvent->runId();
    if(!doRunbyRun && badRuns.count(runID)>0){
        if(doEventDebug)cout<<"Bad run: "<<runID<<endl;
        return false;
    }fillHist1D("hEventStats", 1);

    pVtx = picoEvent->primaryVertex();
    //primary Z vertex cut...
    //if(abs(pVtx.z()) > absZVtx_Max){
    if((pVtx.z() > zVtx_Max) || (pVtx.z() < zVtx_Min)){
        if(doEventDebug)cout<<"Bad Z vertex: "<<pVtx.z()<<endl;
        return false;
    }fillHist1D("hEventStats", 2);

    if(!doppAnalysis){
        int makerReturnInt = runStRefMultCorr();
        if(makerReturnInt >= 0) return false;
    }

    return true;
}

void StPicoAnalysisBaseMaker::addEventToTStarArrays(){
    tsEvent = TStarArrays::addEvent();
    tsEvent->setIdNumbers(runID, picoEvent->eventId());
    tsEvent->setPrimaryVertex(pVtx);
    tsEvent->setRefMults(picoEvent->grefMult(), picoEvent->refMult()); 
    tsEvent->setZDCCoincidence(picoEvent->ZDCx());
    tsEvent->setBBCCoincidence(picoEvent->BBCx());
    tsEvent->setVPDVz(picoEvent->vzVpd());
    tsEvent->setCentrality(centscaled);
    tsEvent->setCorrectedRefmult(correctedRefMult);
    tsEvent->setRefMultWeight(Wt);
    tsEvent->setGenLevelWeight(Wt0);
    tsEvent->setTriggers(picoEvent->triggerIds());
    if(!doppAnalysis){
        if(tsEvent->isMB5()) fillHist1D("hCentralityMB05", centscaled, Wt);
        if(tsEvent->isMB30())fillHist1D("hCentralityMB30", centscaled, Wt);
        if(tsEvent->isHT2()) fillHist1D("hCentralityHT2" , centscaled, Wt);
        if(tsEvent->isHT3()) fillHist1D("hCentralityHT3" , centscaled, Wt);
    }
} 

int StPicoAnalysisBaseMaker::runStRefMultCorr(){
    if(doEventDebug)cout<<"**********StPicoAnalysisBaseMaker::runStRefMultCorr()************"<<endl;

    centscaled = -1;

    if(doppAnalysis){
        if(doEventDebug)cout<<"Doing pp analysis, skipping centrality determination"<<endl;
        return -1;
    }
    if(!grefmultCorr){
        cout<<"ERROR: Doing heavy-ion analysis without StRefMultCorr!"<<endl;
        return -1;
    }
    grefmultCorr->init(runID);
    grefmultCorr->initEvent(picoEvent->grefMult(), pVtx.z(), picoEvent->ZDCx());
    if(doEventDebug)cout<<"grefmultCorr->initEvent() done"<<endl;
  
/*  centrality bins:0: 75-80% 70-80% / 1: 70-75% 60-70% 
                    2: 65-70% 50-60% / 3: 60-65% 40-50% 
                    4: 55-60% 30-40% / 5: 50-55% 20-30% 
                    6: 45-50% 10-20% / 7: 40-45% 5-10% 
                    8: 35-40% 0- 5%  / 9: 30-35% 10 25-30% 
                    11:20-25% / 12: 15-20% / 13: 10-15% 
                    14:5-10% / 15: 0- 5% */
    //centbin9 = grefmultCorr->getCentralityBin9();
    //ref9 = 8-centbin9;
    centbin16 = grefmultCorr->getCentralityBin16();
    if(doEventDebug)cout<<"Got centrality bin..."<<centbin16<<endl;

    if(centbin16 < 0){
        if(doEventDebug)cout<<"Bad centrality bin: "<<centbin16<<endl;
        return kStOK;
    }fillHist1D("hEventStats", 4);

    ref16 = 15-centbin16; 
    centscaled = 5.0*ref16 + 2.5;
    if(doEventDebug)cout<<"Got centrality scaled: "<<centscaled<<"%"<<endl;

    Wt = grefmultCorr->getWeight();
    correctedRefMult = grefmultCorr->getRefMultCorr(picoEvent->grefMult(), pVtx.z(), picoEvent->ZDCx(), 2); 

    if(doEventDebug)cout<<"Got corrected refmult: "<<correctedRefMult<<" Event weight: "<<Wt<<endl;
    
    fillHist1D("hgRefMult", picoEvent->grefMult(), Wt);
    fillHist1D("hRefMult", correctedRefMult, Wt);
    fillHist1D("hCentrality", centscaled, Wt);
    fillHist2D("h2CentvWeight", centscaled, Wt);

    if(doCentSelection){
        if((centscaled < centralityMin) || (centscaled > centralityMax)){
            if(doEventDebug)cout<<"Centrality selection failed: "<<centscaled<<endl;
            return kStOK;
        }fillHist1D("hEventStats", 5); 
    }
    if(doEventDebug)cout<<"********** END StPicoAnalysisBaseMaker::runStRefMultCorr() done************"<<endl;
    return -1;
}


bool StPicoAnalysisBaseMaker::isTrackGood(StPicoTrack* trk){
    if(doTrackDebug)cout<<"**********StPicoAnalysisDataMaker::isTrackGood()************"<<endl;

    if(!trk){
        if(doTrackDebug)cout<<"Track pointer is null!"<<endl;
        return false;
    }
    //if(doDebug){cout<<"Reading StPicoTrack..."<<endl;
        //trk->Print();
    //}       
//Check if track is primary...
    if(!(trk->isPrimary())){
        //if(doDebug)cout<<"Track is not primary!"<<endl;
        return false;
    }fillHist1D("hTrackStats", 0);
//Track quality cuts...
//DCA:
    if(trk->gDCA(pVtx).Mag() > trackDCAMax){
        if(doTrackDebug)
            cout<<"Track failed DCA cut: "<<trk->gDCA(pVtx).Mag()<<endl;
        return false;
    }fillHist1D("hTrackStats", 1);
//nHitsFit:
    if(trk->nHitsFit() < trackNHitsFitMin){
        if(doTrackDebug)
            cout<<"Track failed nHitsFit cut: "<<trk->nHitsFit()<<endl;
        return false;
    }fillHist1D("hTrackStats", 2); 
//nHitsRatio:
    if((trk->nHitsFit()/(double)trk->nHitsMax()) < trackNHitsRatioMin){
        if(doTrackDebug)
            cout<<"Track failed nHitsFit/nHitsMax cut: "<<trk->nHitsFit()/(double)trk->nHitsMax()<<endl;
        return false;
    }fillHist1D("hTrackStats", 3); 

    TVector3 trkMom = trk->pMom();
    double trkPt = trkMom.Pt();
//Track Pt:
    if(trkPt < trackPtMin){
        if(doTrackDebug)
            cout<<"Track failed pt cut: "<<trkPt<<endl;
        return false;
    }fillHist1D("hTrackStats", 4); 
//Track Eta:
    double trkEta = trkMom.Eta();
    if((trkEta < trackEtaMin) || (trkEta > trackEtaMax)){
        if(doTrackDebug)
            cout<<"Track failed eta cut: "<<trkEta<<endl;
        return false;
    }fillHist1D("hTrackStats", 5); 

    fillHist1D("hTrackStats", 6); 

    if(doTrackDebug){
        cout<<"********** END StPicoAnalysisDataMaker::isTrackGood() done************"<<endl;
        cout<<"Track accepted: "<<" pt: "<<trkPt<<" eta: "<<trkEta<<" phi: "<<trkMom.Phi()<<" charge: "<<trk->charge()<<endl;
        if(trkPt > maxTrackPt) cout<<"Max track pt changed from: "<<maxTrackPt<<" to "<<trkPt<<endl;
        cout<<"Tower matched: "<<trk->bemcTowerIndex()<<endl;
        //cout<<"Tracking efficiency: "<<getTrackingEfficiency(trkPt, trkEta, ref16, tsEvent->ZDC_Coincidence(), efficiencyFile)<<endl;
        cout <<"NSigma Pion: "<< trk->nSigmaPion();
        cout <<" Kaon: "      << trk->nSigmaKaon();
        cout <<" Proton: "    << trk->nSigmaProton();
        cout <<" Electron: "  << trk->nSigmaElectron()<<endl;
    }

    return true;
}

bool StPicoAnalysisBaseMaker::isTowerGood(unsigned int itow, StPicoBTowHit* tower){
    if(doTowerDebug)cout<<"**********StPicoAnalysisDataMaker::isTowerGood()************"<<endl;

    if(!tower){
        //if(doDebug)cout<<"Tower pointer is null!"<<endl;
        return false;
    }fillHist1D("hTowerStats", 0);

    if(tower->isBad()){
        //if(doDebug)cout<<"Tower is bad ! 1"<<endl;
        return false;
    }
    if(badTowers.count(itow+1)>0){
        //if(doDebug)cout<<"Tower is bad ! 2"<<endl;
        return false;
    }fillHist1D("hTowerStats", 1);

    if(deadTowers.count(itow+1)>0){
        //if(doDebug)cout<<"Tower is dead!"<<endl;
        return false;
    }fillHist1D("hTowerStats", 2);

    if(tower->energy() < towerEnergyMin){
        if(doTowerDebug)
            cout<<"Tower failed energy cut: "<<tower->energy()<<endl;
        return false;
    }fillHist1D("hTowerStats", 3);

     //Get tower's position...
    towPos = emcPosition->getPosFromVertex(pVtx, itow+1);
    if(doTowerDebug){
        cout<<"Tower: "<<itow<<" E: "<<tower->energy()<<" eta: "<<towPos.Eta()<<" phi: "<<towPos.Phi()<<endl;
    }
    double towEta = towPos.Eta();
    if((towEta < towerEtaMin) || (towEta > towerEtaMax)){
        if(doTowerDebug)
            cout<<"Tower eta: "<<towEta<<" outside of range: "<<towerEtaMin<<" to "<<towerEtaMax<<endl;
        return false;
    }fillHist1D("hTowerStats", 4);

    if(doTowerDebug)cout<<"********** END StPicoAnalysisDataMaker::isTowerGood() done************"<<endl;

    return true;
}

double StPicoAnalysisBaseMaker::getTrackingEfficiency(double x, double y, int cbin, double zdcx, TFile *infile){
    if(!infile)return -99;

    double effBinContent = -99; // value to be extracted from histogram
    const char *species =  "pion"; // FIXME
    int lumiBin = floor(zdcx/10000);

    char* histName; 
    int effBin = -99;

    if(!doppAnalysis){
        if(x > 4.5) x = 4.5;  // for pt > 4.5 use value at 4.5
        // get 2D pTEta efficiency histo
        histName = Form("hTrack_%s_Efficiency_pTEta_final_centbin%d_lumibin%d", species, cbin, lumiBin);
    }else{
       if(x > 1.8) x = 1.8;  // for pt > 1.8 use value at 1.8
       histName = Form("hppRun12_PtEtaEfficiency_data_aacuts");
    }

    TH2F *h = static_cast<TH2F*>(infile->Get(Form("%s", histName)));
    if(!h) cout<<"don't have requested histogram! "<<Form("%s", histName)<<endl;
    h->SetName(Form("%s", histName));
    // get efficiency 
    effBin        = h->FindBin(x, y); // pt, eta
    effBinContent = h->GetBinContent(effBin);
    // delete histo and close input file
    delete h;

    return effBinContent;
}


void StPicoAnalysisBaseMaker::addHist1D(const std::string& key, const std::string& title, const int& nBinsX, const double& xMin, const double& xMax){
    histos1D[key] = new TH1F(key.c_str(), title.c_str(), nBinsX, xMin, xMax);
}

void StPicoAnalysisBaseMaker::addHist1D(const std::string& key, const std::string& title, const int& nBinsX, double* xBins){
    histos1D[key] = new TH1F(key.c_str(), title.c_str(), nBinsX, xBins);
}

void StPicoAnalysisBaseMaker::addHist2D(const std::string& key, const std::string& title, const int& nBinsX, const double& xMin, const double& xMax, const int& nBinsY, const double& yMin, const double& yMax){
    histos2D[key] = new TH2F(key.c_str(), title.c_str(), nBinsX, xMin, xMax, nBinsY, yMin, yMax);
}

void StPicoAnalysisBaseMaker::addHist2D(const std::string& key, const std::string& title, const int& nBinsX, double* xBins, const int& nBinsY, const double& yMin, const double& yMax){
    histos2D[key] = new TH2F(key.c_str(), title.c_str(), nBinsX, xBins, nBinsY, yMin, yMax);
}

void StPicoAnalysisBaseMaker::addHist2D(const std::string& key, const std::string& title, const int& nBinsX, const double& xMin, const double& xMax, const int& nBinsY, double* yBins){
    histos2D[key] = new TH2F(key.c_str(), title.c_str(), nBinsX, xMin, xMax, nBinsY, yBins);
}

void StPicoAnalysisBaseMaker::addHist2D(const std::string& key, const std::string& title, const int& nBinsX, double* xBins, const int& nBinsY, double* yBins){
    histos2D[key] = new TH2F(key.c_str(), title.c_str(), nBinsX, xBins, nBinsY, yBins);
}

void StPicoAnalysisBaseMaker::fillHist1D(const std::string& key, const double& x, const double& weight){
    auto search = histos1D.find(key);
    if(search != histos1D.end())  search->second->Fill(x, weight);
}

void StPicoAnalysisBaseMaker::fillHist2D(const std::string& key, const double& x, const double& y, const double& weight){
    auto search = histos2D.find(key);
    if(search != histos2D.end()) search->second->Fill(x, y, weight);
}

void StPicoAnalysisBaseMaker::declareStatHistos(){
    addHist1D("hEventStats", "Event Statistics", 11, -0.5, 10.5);    
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(1, "ALL");
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(2, "RUN GOOD");
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(3, "VZ PASS");
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(4, "HAS TRIGGER");
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(5, "CENTRALITY GOOD");
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(6, "CENTRALITY PASS");
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(7, "HAS JET CONSTIT");
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(8, "PT MAX PASS");
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(9, "HAS JET");
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(10,"HAS GEN JET");
    getHist1D("hEventStats")->GetXaxis()->SetBinLabel(11,"GOOD");

}

void StPicoAnalysisBaseMaker::setUpBadRuns(){
    ifstream inFile(badRunsList.c_str());

    if( !inFile.good() ) {
        cout << "Can't open " << badRunsList <<endl;;
    }else cout<<"Opening Bad Runs file: "<<badRunsList<<endl;

    string line;
    while(getline (inFile, line) ){
        if( line.size()==0 ) continue; // skip empty lines
        if( line[0] == '#' ) continue; // skip comments
        istringstream ss( line );
        while( ss ){
            string entry;
            getline( ss, entry, ',' );
            int ientry = atoi(entry.c_str());
            if(ientry) {
                badRuns.insert( ientry );
                //cout << "Added bad run # "<< ientry<< endl;
            }
        }
    }
}

void StPicoAnalysisBaseMaker::setUpBadTowers(){
    ifstream inFile (badTowersList.c_str());
    if( !inFile.good() ) {
        cout << "Can't open " << badTowersList <<endl;
    }else cout<<"Opening Bad Tower file: "<<badTowersList<<endl;
    string line;
    while(getline (inFile, line) ){
        if( line.size()==0 ) continue; // skip empty lines
        if( line[0] == '#' ) continue; // skip comments
        istringstream ss( line );
        while( ss ){
            string entry;
            getline( ss, entry, ',' );
            int ientry = atoi(entry.c_str());
            if(ientry) {
                badTowers.insert( ientry );
                //cout << "Added bad tower # "<< ientry<< endl;
            }
        }
    }
}

void StPicoAnalysisBaseMaker::setUpDeadTowers(){
    ifstream inFile ( deadTowersList.c_str() );
    if( !inFile.good() ) {
        cout << "Can't open " << deadTowersList <<endl;;
    }else cout<<"Opening Dead Tower file: "<<deadTowersList<<endl;
    string line;
    while(getline (inFile, line) ){
        if( line.size()==0 ) continue; // skip empty lines
        if( line[0] == '#' ) continue; // skip comments
        istringstream ss( line );
        while( ss ){
            string entry;
            getline( ss, entry, ',' );
            int ientry = atoi(entry.c_str());
            if(ientry) {
                deadTowers.insert( ientry );
                //cout << "Added dead tower # "<< ientry<< endl;
            }
        }
    }
}
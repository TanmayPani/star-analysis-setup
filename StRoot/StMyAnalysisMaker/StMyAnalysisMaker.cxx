#define StMyAnalysisMaker_cxx

#include "StMyAnalysisMaker.h"
//#include <unordered_set>
//ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
// centrality includes
#include "StRoot/StRefMultCorr/CentralityMaker.h"
#include "StRoot/StRefMultCorr/StRefMultCorr.h"
// STAR includes
#include "StPicoEvent/StPicoDst.h"
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoTrack.h"
//#include "StRoot/StPicoEvent/StPicoEmcTrigger.h"
#include "StPicoEvent/StPicoBTowHit.h"
#include "StPicoEvent/StPicoBEmcPidTraits.h"
//MyAnalysisMaker includes
#include "StEmcPosition2.h"
//#include "StMyAnalysisUtils.h"
#include "StRoot/TStarEventClass/TStarEvent.h"
#include "StRoot/TStarEventClass/TStarTrack.h"
#include "StRoot/TStarEventClass/TStarTower.h"

ClassImp(StMyAnalysisMaker);

using namespace std;

StMyAnalysisMaker::StMyAnalysisMaker(string name, string output, bool dodebug):
StMaker(name.c_str()){
    doDebug = dodebug;

    if(doDebug)cout<<"StMyAnalysisMaker::StMyAnalysisMaker()"<<endl;

    anaName = name; 
    outputFileName = "EventTree_"+output;
    histoFileName = "Histograms_"+output;

    if(doDebug){
        cout<<"Name of the StMaker instance : "<<anaName<<endl;
        cout<<"Name of the file that will store the output TTree : "<<outputFileName<<endl;
        cout<<"Name of the file that will store the Histograms : "<<histoFileName<<endl;
    }

    towerHadCorrSumTrE.resize(4800);
    towerHadCorrMaxTrE.resize(4800);
    towerNTracksMatched.resize(4800);
}

StMyAnalysisMaker::~StMyAnalysisMaker(){
    if(doDebug)cout<<"********StMyAnalysisMaker::~StMyAnalysisMaker()*********"<<endl;
    if(emcPosition) delete emcPosition;
    if(efficiencyFile) delete efficiencyFile; 
    if(!tsEvent->TestBit(kIsOnHeap))delete tsEvent;
    for(const auto& hist1D : histos1D){
        if(doDebug)cout<<"Deleting "<<hist1D.first<<endl;
        if(hist1D.second) delete hist1D.second;
    }
    for(const auto& hist2D : histos2D){
        if(doDebug)cout<<"Deleting "<<hist2D.first<<endl;
        if(hist2D.second) delete hist2D.second;
    }
    histos1D.clear();
    histos2D.clear();
}

Int_t StMyAnalysisMaker::Init(){
    if(doDebug)cout<<"***********StMyAnalysisMaker::Init()**************"<<endl;
    if(!doppAnalysis){
        grefmultCorr = CentralityMaker::instance()->getgRefMultCorr_P18ih_VpdMB30_AllLumi();
        if(doDebug){
            cout<<"Set up grefmultCorr..."<<endl;
            grefmultCorr->print();
        }

        grefmultCorrUtil = new StRefMultCorr("grefmult_P18ih_VpdMB30_AllLumi_MB5sc");
        grefmultCorrUtil->setVzForWeight(16, -16.0, 16.0);
        grefmultCorrUtil->readScaleForWeight("StRoot/StRefMultCorr/macros/weight_grefmult_vpd30_vpd5_Run14_P18ih_set1.txt");
    }

    if(!doRunbyRun)setUpBadRuns(); //Probably deprecated, might add this into StRefMultCorr

    setUpBadTowers();//There may be a better way
    setUpDeadTowers();//There may be a better way
    declareHistograms();

    string efficiencyFileName;

    if(doppAnalysis)efficiencyFileName = "./StRoot/StMyAnalysisMaker/Run12_efficiency_New.root";
    else efficiencyFileName = "./StRoot/StMyAnalysisMaker/Run14_efficiencySmaller2D.root";

    efficiencyFile = new TFile(efficiencyFileName.c_str(), "READ");
    
    if(!efficiencyFile->IsOpen()){
        cout<<"Could not open efficiency file!"<<endl;
        return kStWarn;
    }
    if(doDebug)cout<<"Opened efficiency file: "<<efficiencyFileName<<endl;

    if(doDebug)cout<<"Setting up StEmcPosition2..."<<endl;
    emcPosition = new StEmcPosition2();

    if(doDebug)cout<<"Setting up TStarEvent..."<<endl;
    tsEvent = new TStarEvent();

    bookTree();

    if(doDebug)cout<<"***********END OF StMyAnalysisMaker::Init()**************"<<endl;

   // if(treeOut->IsOpen())   treeOut->Close();

    return kStOK;
}

void StMyAnalysisMaker::writeHistograms(){
    if(histoFileName != ""){
        histOut = new TFile(histoFileName.c_str(), "UPDATE");
        histOut->cd();
        for(const auto& hist : histos1D){
            hist.second->Write();
            if(doDebug)cout<<"Wrote "<<hist.first<<endl;
        }
        for(const auto& hist : histos2D){
            hist.second->Write();
            if(doDebug)cout<<"Wrote "<<hist.first<<endl;
        }
        histOut->Write();
        histOut->Close();
        if(doDebug)cout<<"Wrote histograms to "<<histoFileName<<endl;
    }
}

Int_t StMyAnalysisMaker::Finish(){
    cout<< "StMyAnalysisMaker::Finish()"<<endl;

    if(outputFileName != ""){
        if(doDebug)cout<<"Writing tree to "<<outputFileName<<endl;
        //treeOut->cd();
        treeOut->Write();
        treeOut->Close();
    }

    writeHistograms(); 

    efficiencyFile->Close();

    TStarEvent::Class()->IgnoreTObjectStreamer();
    TStarVector::Class()->IgnoreTObjectStreamer();

    return kStOk;
}

Int_t StMyAnalysisMaker::Make(){
    if(doDebug)cout<<"***********StMyAnalysisMaker::Make()**************"<<endl;

    tsEvent->clearEvent();

    picoDstMaker = static_cast<StPicoDstMaker*>(GetMaker("picoDst"));
    if(!picoDstMaker){
        cout<<"You havent added a StPicoDstMaker!"<<endl;
        return kStFatal;
    }if(doDebug)cout<<"Got StPicoDstMaker!"<<endl;

    picoDst = static_cast<StPicoDst*>(picoDstMaker->picoDst());
    if(!picoDst){
        cout << " No PicoDst! Skip! " << endl;
        return kStWarn;
    }if(doDebug)cout<<"Got StPicoDst!"<<endl;

    picoEvent = static_cast<StPicoEvent*>(picoDst->event());
    if(!picoEvent){
        cout<<" No PicoEvent! Skip! " << endl;
        return kStWarn;
    }if(doDebug){
        cout<<"Got StPicoEvent!"<<endl;
    }

    if(doDebug){
        cout<<"************ StPicoDst::print() *************"<<endl;
        picoDst->print();
        //picoDst->printTracks();
        //picoDst->printBTowHits();
        cout<<"************ END StPicoDst::print() *************"<<endl;
    }

    histos1D["hEventStats"]->Fill(0);

    //Reject bad runs here..., if doing run by run jobs, reject bad runs while submitting jobs
    runID = picoEvent->runId();
    if(!doRunbyRun && badRuns.count(runID)>0){
        if(doDebug)cout<<"Bad run: "<<runID<<endl;
        return kStOK;
    }
    histos1D["hEventStats"]->Fill(1);

    pVtx = picoEvent->primaryVertex();
    //primary Z vertex cut...
    if(abs(pVtx.z()) > absZVtx_Max){
        if(doDebug)cout<<"Bad Z vertex: "<<pVtx.z()<<endl;
        return kStOK;
    }histos1D["hEventStats"]->Fill(2);

    tsEvent->setIdNumbers(runID, picoEvent->eventId());
    tsEvent->setPrimaryVertex(pVtx);
    tsEvent->setRefMults(picoEvent->grefMult(), picoEvent->refMult()); 
    tsEvent->setZDCCoincidence(picoEvent->ZDCx());

    //bool isTriggerPresent = setUpEventTriggers();
    //if(!isTriggerPresent){return kStOK;}

    eventTriggers = picoEvent->triggerIds();
    if(doDebug){
        cout<<"Reading event Triggers: "<<endl;
        for(const auto& trig : eventTriggers){
            cout<<trig<<endl;
        }
    }

    tsEvent->setTriggers(eventTriggers);
    histos1D["hTriggerStats"]->Fill(0); 
    if(tsEvent->isMBmon())histos1D["hTriggerStats"]->Fill(1);
    if(tsEvent->isMB5())  histos1D["hTriggerStats"]->Fill(2);
    if(tsEvent->isMB30()) histos1D["hTriggerStats"]->Fill(3);
    if(tsEvent->isHT1())  histos1D["hTriggerStats"]->Fill(4);
    if(tsEvent->isHT2())  histos1D["hTriggerStats"]->Fill(5);
    if(tsEvent->isHT3())  histos1D["hTriggerStats"]->Fill(6);

    if(doSelectHTEventsOnly){
        if(!tsEvent->isHT()){
            if(doDebug)cout<<"Not a HT event!"<<endl;
            return kStOK;
        }histos1D["hEventStats"]->Fill(3);
    }
 
    if(!doppAnalysis){
        bool isCentralityOk = runStRefMultCorr();
        if(!isCentralityOk) return kStOK;
    }

    histos1D["hgRefMult"]->Fill(picoEvent->grefMult(), Wt);

    tsEvent->setBBCCoincidence(picoEvent->BBCx());
    tsEvent->setVPDVz(picoEvent->vzVpd());

    //RunOverEmcTriggers(); //Runs over all emc i.e., High tower (and Jet patch for pp) triggers... 
    tsEvent->clearTrackArray();
    runOverTracks(); //Runs over all tracks
    tsEvent->clearTowerArray();
    runOverTowers(); //Runs over all towers

    if(doDebug)
        cout<<"Max track pt: "<<maxTrackPt<<" Max tower Et: "<<maxTowerEt<<endl;

    histos2D["h2MaxTrkPtvTowEt"]->Fill(maxTrackPt, maxTowerEt, Wt);

    if(excludeLowEnergyEvents){
        if((maxTrackPt < jetConstituentMinPt) && (maxTowerEt < jetConstituentMinPt)){
            if(doDebug)cout<<"Rejected low energy event: "<<maxTrackPt<<" "<<maxTowerEt<<endl;
            return kStOK;
        }histos1D["hEventStats"]->Fill(6);
    }

    if(excludeLowChargedEnergyEvents){
        if(maxTrackPt < minTrackPtMax){
            if(doDebug)cout<<"Rejected low charged energy event: "<<maxTrackPt<<endl;
            return kStOK;
        }histos1D["hEventStats"]->Fill(7);
    }

    histos1D["hEventStats"]->Fill(8);

    if(doDebug){
        cout<<"TStarEvent summary: "<<endl;
        tsEvent->print();
        cout<<"Filling tree"<<endl;
    }
    tree->Fill();

    if(doDebug)cout<<"**************Finished StMyAnalysisMaker::Make()********************"<<endl;

    return kStOk;
}

void StMyAnalysisMaker::bookTree(){
    if(outputFileName == ""){
        cout<<"Trees are not being written to any file!"<<endl;
        tree = new TTree("Events", "Tree with event Info");
        tree->Branch("Event", &(tsEvent)); 
    }else{
        treeOut = new TFile(outputFileName.c_str(), "UPDATE");
        treeOut->cd();
        //treeOut->mkdir(GetName());
        //writedir = (TDirectory*)treeOut->Get(GetName());
        cout<<"Writing tree to: "<<outputFileName<<endl;
        tree = new TTree("Events", "Tree with event Info");
        tree->SetDirectory(gDirectory);
        tree->Branch("Event", &(tsEvent)); 
        cout<<"Events tree directory set"<<endl;
    }
}

bool StMyAnalysisMaker::runStRefMultCorr(){
    if(doDebug)cout<<"**********StMyAnalysisMaker::runStRefMultCorr()************"<<endl;
    if(!grefmultCorr){
        cout<<"WARNING: Doing heavy-ion analysis without StRefMultCorr!"<<endl;
        return false;
    }
    grefmultCorr->init(runID);
    grefmultCorr->initEvent(tsEvent->gRefMult(), tsEvent->Vz(), tsEvent->ZDC_Coincidence());
    if(doDebug)cout<<"grefmultCorr->initEvent() done"<<endl;
/*  
    centrality bins:0 75-80% 70-80% 
                    1 70-75% 60-70% 
                    2 65-70% 50-60% 
                    3 60-65% 40-50% 
                    4 55-60% 30-40% 
                    5 50-55% 20-30% 
                    6 45-50% 10-20% 
                    7 40-45% 5-10% 
                    8 35-40% 0- 5% 
                    9 30-35% 
                    10 25-30% 
                    11 20-25% 
                    12 15-20% 
                    13 10-15% 
                    14 5-10% 
                    15 0- 5% 
*/
    //centbin9 = grefmultCorr->getCentralityBin9();
    //ref9 = 8-centbin9;
    centbin16 = grefmultCorr->getCentralityBin16();
    if(doDebug)cout<<"Got centrality bin..."<<centbin16<<endl;

    if(centbin16 < 0){
        if(doDebug)cout<<"Bad centrality bin: "<<centbin16<<endl;
        return false;
    }histos1D["hEventStats"]->Fill(4);

    ref16 = 15-centbin16; 
    centscaled = 5.0*ref16;
    if(doDebug)cout<<"Got centrality scaled: "<<centscaled<<endl;

    tsEvent->setCentrality(centscaled);

    tsEvent->setCorrectedRefmult(grefmultCorr->getRefMultCorr(tsEvent->gRefMult(), tsEvent->Vz(), tsEvent->ZDC_Coincidence(), 2));
    Wt = grefmultCorr->getWeight();
    tsEvent->setWeight(Wt);

    if(doDebug)cout<<"Got corrected refmult: "<<tsEvent->refMultCorr()<<" Event weight: "<<Wt<<endl;

    histos1D["hRefMult"]->Fill(tsEvent->refMultCorr(), Wt);

    histos1D["hCentrality"]->Fill(centscaled);
    if(tsEvent->isMB5()) histos1D["hCentralityMB05"]->Fill(centscaled);
    if(tsEvent->isMB30())histos1D["hCentralityMB30"]->Fill(centscaled);
    if(tsEvent->isHT2()) histos1D["hCentralityHT2" ]->Fill(centscaled);
    if(tsEvent->isHT3()) histos1D["hCentralityHT3" ]->Fill(centscaled);
    histos2D["h2CentvWeight"]->Fill(centscaled, Wt);

    if(doCentSelection){
        if((centscaled < centralityMin) || (centscaled > centralityMax)){
            if(doDebug)cout<<"Centrality selection failed: "<<centscaled<<endl;
            return false;
        }histos1D["hEventStats"]->Fill(5); 
    }

    //grefmultCorrUtil->init(runID);
    //grefmultCorrUtil->initEvent(tsEvent->gRefMult(), tsEvent->Vz(), tsEvent->ZDC_Coincidence()); 
    //tsEvent->SetMB5toMB30Reweight((tsEvent->IsMB5() && !tsEvent->IsMB30()) ? grefmultCorrUtil->getWeight() : 1.0);
    if(doDebug)cout<<"********** END StMyAnalysisMaker::runStRefMultCorr() done************"<<endl;
    return true;
}

void StMyAnalysisMaker::runOverTracks(){
    if(doDebug)cout<<"**********StMyAnalysisMaker::runOverTracks()************"<<endl;

    if(doDebug)cout<<"Resetting hadronic correction vectors... "<<endl;
    towerHadCorrSumTrE.assign(4800, 0);
    towerHadCorrMaxTrE.assign(4800, 0);
    towerNTracksMatched.assign(4800, 0);

    if(maxTrackPt > 0){
        if(doDebug)cout<<"Max track pt was at "<<maxTrackPt<<", now set to 0 ..."<<endl;
        maxTrackPt = 0;
    }

    if(tsEvent->Tracks->GetEntriesFast() > 0){
        cout<<"Track array not cleared from previous event!"<<endl;
    }

    unsigned int nTracks = picoDst->numberOfTracks();

    if(doDebug){
        cout<<"*************** Tracks Summary: ***************"<<endl;
        cout<<"Number of tracks: "<<nTracks<<endl;
    }

    for(unsigned int itrk = 0; itrk < nTracks; itrk++){ //begin Track Loop...
        StPicoTrack *trk = static_cast<StPicoTrack*>(picoDst->track(itrk));

        if(!isTrackGood(trk)) continue;

        TVector3 trkMom = trk->pMom();
        double trkPt = trkMom.Perp();
        double trkEta = trkMom.Eta();
        double trkPhi = trkMom.Phi();
        short trkChrg = trk->charge();

        if(doDebug){
        cout<<"Track: "<<itrk<<" pt: "<<trkPt<<" eta: "<<trkEta<<" phi: "<<trkPhi<<" charge: "<<trkChrg<<endl;
        if(trkPt > maxTrackPt)
            cout<<"Max track pt changed from: "<<maxTrackPt<<" to "<<max(trkPt, maxTrackPt)<<endl;
        }

        maxTrackPt = max(trkPt, maxTrackPt);

        TStarTrack tsTrk(itrk, trkChrg, trkMom, pi0mass);

        double trackingEff = getTrackingEfficiency(trkPt, trkEta, ref16, tsEvent->ZDC_Coincidence(), efficiencyFile);
        tsTrk.setTrackingEfficiency(trackingEff);

        int towerMatched = trk->bemcTowerIndex();
        tsTrk.setMatchedTower(towerMatched);
        tsTrk.setNSigmas(trk->nSigmaPion(), trk->nSigmaKaon(), trk->nSigmaProton(), trk->nSigmaElectron());

        if(doDebug){
            cout<<"Tower matched: "<<towerMatched<<" Tracking efficiency: "<<trackingEff<<endl;
            cout<<"NSigma Pion: "<<trk->nSigmaPion()<<" Kaon: "<<trk->nSigmaKaon()<<" Proton: "<<trk->nSigmaProton()<<" Electron: "<<trk->nSigmaElectron()<<endl;
        }

        if(towerMatched >= 0){
            towerNTracksMatched[towerMatched]++;
            double E = sqrt(trkMom.Mag2() + pi0mass*pi0mass);
            towerHadCorrSumTrE[towerMatched] += E;
            towerHadCorrMaxTrE[towerMatched] = max(E, towerHadCorrMaxTrE[towerMatched]);
        }else{histos1D["hTrackStats"]->Fill(7);}

        if(doDebug){
            cout<<"_______Done with track: "<<itrk<<"________"<<endl;
            cout<<"Made StPicoTrack into TStarTrack! comparing"<<endl;
            tsTrk.print();
            cout<<"Adding to track array..."<<endl;
        }

        tsEvent->addTrack(tsTrk);
 
        histos1D["hTrackPt"]->Fill(trkPt, Wt);
        histos1D["hTrackPtxCh"]->Fill(trkPt*trkChrg, Wt);
        histos1D["hTrackEta"]->Fill(trkEta, Wt); 
        histos1D["hTrackPhi"]->Fill(tsTrk.phi(), Wt);

        histos2D["h2TrackPtvEff"]->Fill(trkPt, trackingEff, Wt);
        histos2D["h2TrackEtavPhi"]->Fill(tsTrk.phi(), trkEta, trkPt*Wt); 
    } //end Track Loop...
    histos2D["h2CentvMaxTrackPt"]->Fill(centscaled, maxTrackPt, Wt);
    if(doDebug)cout<<"Final Max track pt: "<<maxTrackPt<<endl;
    tsEvent->setMaxTrackPt(maxTrackPt);
    if(doDebug)cout<<"********** END StMyAnalysisMaker::runOverTracks() done************"<<endl;
}

void StMyAnalysisMaker::runOverTowers(){    
    if(doDebug)cout<<"**********StMyAnalysisMaker::runOverTowers()************"<<endl;

    if(maxTowerEt > 0){
        if(doDebug)cout<<"Max tower Et was at "<<maxTowerEt<<", now set to 0..."<<endl;
        maxTowerEt = 0;
    }

    if(tsEvent->Towers->GetEntriesFast() > 0){
        cout<<"Tower array not cleared from previous event!"<<endl;
    }

    unsigned int nTowers = picoDst->numberOfBTowHits();

    if(doDebug){
        cout<<"*************** Towers Summary: ***************"<<endl;
        cout<<"Number of towers: "<<nTowers<<endl;
    }

    for(unsigned int itow = 0; itow < nTowers; itow++){
        StPicoBTowHit *tower = static_cast<StPicoBTowHit*>(picoDst->btowHit(itow));

        if(!isTowerGood(itow, tower)) continue;

        double towERaw = tower->energy();

        //Get tower's position...
        TVector3 towPos = emcPosition->getPosFromVertex(pVtx, itow+1);

        if(doDebug){
            cout<<"Tower: "<<itow<<" E: "<<towERaw<<" eta: "<<towPos.Eta()<<" phi: "<<towPos.Phi()<<endl;
        }

        double towEta = towPos.Eta();
        if((towEta < towerEtaMin) || (towEta > towerEtaMax)){
            if(doDebug)
                cout<<"Tower eta: "<<towEta<<" outside of range: "<<towerEtaMin<<" to "<<towerEtaMax<<endl;
            continue;
        }histos1D["hTowerStats"]->Fill(4);

        //Start hardonic correction of tower...
        double towE = towERaw;
        if(towerNTracksMatched[itow] != 0){
            if(doDebug){
                cout<<"Tower: "<<itow<<" had "<<towerNTracksMatched[itow]<<" tracks matched ";
                cout<<"sum of energies"<<towerHadCorrSumTrE[itow]<<" max energy: "<<towerHadCorrMaxTrE[itow]<<endl;
            }
            if(hadronicCorrType == HadronicCorrectionType::kFull){towE -= towerHadCorrSumTrE[itow];}
            else if(hadronicCorrType == HadronicCorrectionType::kHighestMatchedTrackE){towE -= towerHadCorrMaxTrE[itow];}
        }else{histos1D["hTowerStats"]->Fill(7);}

        if(doDebug)
            cout<<"Tower energy after hadronic correction: "<<towE<<endl;
        
        double towEt = towE/cosh(towPos.Eta());
        if(towEt < towerEnergyMin){
        if(doDebug)
            cout<<"Tower Et: "<<towEt<<" less than minimum: "<<towerEnergyMin<<endl;
            continue;
        }histos1D["hTowerStats"]->Fill(5);

        //if(towEt > maxTowerEt)cout<<"Max tower Et changed from: "<<maxTowerEt<<" to "<<max(towEt, maxTowerEt)<<endl;

        maxTowerEt = max(towEt, maxTowerEt);

        histos1D["hTowerStats"]->Fill(6);

        TStarTower tsTow(itow, tower->adc(), towERaw, towE, towPos, pi0mass);
        tsTow.setNMatchedTracks(towerNTracksMatched[itow]);

        if(doDebug){
            cout<<"Made StPicoBTowHit into TStarTower! comparing"<<endl;
            tsTow.print();
            cout<<"Adding to tower array..."<<endl;
        }

        tsEvent->addTower(tsTow);

        histos1D["hTowerERaw"]->Fill(towERaw, Wt);
        histos1D["hTowerE"]->Fill(towE, Wt);
        histos1D["hTowerEt"]->Fill(towEt, Wt);
        histos1D["hTowerEta"]->Fill(towEta, Wt);
        histos1D["hTowerPhi"]->Fill(tsTow.phi(), Wt);
        histos2D["hTowerdESumvMax"]->Fill(towerHadCorrSumTrE[itow], towerHadCorrMaxTrE[itow], Wt);
        histos2D["h2TowerEtavPhi"]->Fill(tsTow.phi(), towEta, towEt*Wt);

    }//end tower loop...

    histos2D["h2CentvMaxTowerEt"]->Fill(centscaled, maxTowerEt, Wt);
    if(doDebug)cout<<"Final Max tower Et: "<<maxTowerEt<<endl;
    tsEvent->setMaxTowerEt(maxTowerEt);
    if(doDebug)cout<<"********** END StMyAnalysisMaker::runOverTowers() done************"<<endl;
}

bool StMyAnalysisMaker::isTowerGood(unsigned int itow, StPicoBTowHit* tower){
    if(doDebug)cout<<"**********StMyAnalysisMaker::isTowerGood()************"<<endl;

    if(!tower){
        //if(doDebug)cout<<"Tower pointer is null!"<<endl;
        return false;
    }histos1D["hTowerStats"]->Fill(0);

    if(tower->isBad()){
        //if(doDebug)cout<<"Tower is bad ! 1"<<endl;
        return false;
    }
    if(badTowers.count(itow+1)>0){
        //if(doDebug)cout<<"Tower is bad ! 2"<<endl;
        return false;
    }histos1D["hTowerStats"]->Fill(1);

    if(deadTowers.count(itow+1)>0){
        //if(doDebug)cout<<"Tower is dead!"<<endl;
        return false;
    }histos1D["hTowerStats"]->Fill(2);

    if(tower->energy() < towerEnergyMin){
        if(doDebug)
            cout<<"Tower failed energy cut: "<<tower->energy()<<endl;
        return false;
    }histos1D["hTowerStats"]->Fill(3);

    if(doDebug)cout<<"********** END StMyAnalysisMaker::isTowerGood() done************"<<endl;

    return true;
}


bool StMyAnalysisMaker::isTrackGood(StPicoTrack* trk){
    if(doDebug)cout<<"**********StMyAnalysisMaker::isTrackGood()************"<<endl;

    if(!trk){
        if(doDebug)cout<<"Track pointer is null!"<<endl;
        return false;
    }
    //if(doDebug){cout<<"Reading StPicoTrack..."<<endl;
        //trk->Print();
    //}       
//Check if track is primary...
    if(!(trk->isPrimary())){
        //if(doDebug)cout<<"Track is not primary!"<<endl;
        return false;
    }histos1D["hTrackStats"]->Fill(0);
//Track quality cuts...
//DCA:
    if(trk->gDCA(pVtx).Mag() > trackDCAMax){
        if(doDebug)
            cout<<"Track failed DCA cut: "<<trk->gDCA(pVtx).Mag()<<endl;
        return false;
    }histos1D["hTrackStats"]->Fill(1);
//nHitsFit:
    if(trk->nHitsFit() < trackNHitsFitMin){
        if(doDebug)
            cout<<"Track failed nHitsFit cut: "<<trk->nHitsFit()<<endl;
        return false;
    }histos1D["hTrackStats"]->Fill(2); 
//nHitsRatio:
    if((trk->nHitsFit()/(double)trk->nHitsMax()) < trackNHitsRatioMin){
        if(doDebug)
            cout<<"Track failed nHitsFit/nHitsMax cut: "<<trk->nHitsFit()/(double)trk->nHitsMax()<<endl;
        return false;
    }histos1D["hTrackStats"]->Fill(3); 

    TVector3 trkMom = trk->pMom();
    double trkPt = trkMom.Pt();
//Track Pt:
    if(trkPt < trackPtMin){
        if(doDebug)
            cout<<"Track failed pt cut: "<<trkPt<<endl;
        return false;
    }histos1D["hTrackStats"]->Fill(4); 
//Track Eta:
    double trkEta = trkMom.Eta();
    if((trkEta < trackEtaMin) || (trkEta > trackEtaMax)){
        if(doDebug)
            cout<<"Track failed eta cut: "<<trkEta<<endl;
        return false;
    }histos1D["hTrackStats"]->Fill(5); 

    histos1D["hTrackStats"]->Fill(6); 

    if(doDebug)cout<<"********** END StMyAnalysisMaker::isTrackGood() done************"<<endl;

    return true;
}


//void StMyAnalysisMaker::RunOverTowerClusters(){
//Will put methods relating to analyzing objects from StPicoBEmcPidTraits...
//}

void StMyAnalysisMaker::declareHistograms(){
    if(doDebug)cout<<"**********StMyAnalysisMaker::declareHistograms()************"<<endl;

    histos1D["hEventStats"]     = new TH1F("hEventStats", "Event Statistics", 10, -0.5, 9.5);
    histos1D["hTriggerStats"]     = new TH1F("hTriggerStats", "Trigger Statistics", 10, -0.5, 9.5);
    histos1D["hRefMult"] = new TH1F("hRefMult", "Reference Multiplicity", 701, -0.5, 700.5);
    histos1D["hgRefMult"] = new TH1F("hgRefMult", "Global Reference Multiplicity", 701, -0.5, 700.5);
    histos1D["hCentrality"]     = new TH1F("hCentrality", "Event Centrality", 16, 0, 80);
    histos1D["hCentralityMB05"] = new TH1F("hCentralityMB05", "Event Centrality for MB5 events", 16, 0, 80); 
    histos1D["hCentralityMB30"] = new TH1F("hCentralityMB30", "Event Centrality for MB30 events", 16, 0, 80); 
    histos1D["hCentralityHT2"]  = new TH1F("hCentralityHT2", "Event Centrality for HT2 events", 16, 0, 80); 
    histos1D["hCentralityHT3"]  = new TH1F("hCentralityHT3", "Event Centrality for HT3 events", 16, 0, 80);

    histos2D["h2CentvWeight"] = new TH2F("h2CentvWeight", "Event Centrality vs RefMultCorr Weight", 16, 0, 80, 25, 0, 2.5);
    histos2D["h2CentvMaxTrackPt"] = new TH2F("h2CentvMaxTrackPt", "Event Centrality vs max(p_{T, track})", 16, 0, 80, 60, 0, 30);
    histos2D["h2CentvMaxTowerEt"] = new TH2F("h2CentvMaxTowerEt", "Event Centrality vs max(E_{T, tower})", 16, 0, 80, 80, 0, 40);
    histos2D["h2MaxTrkPtvTowEt"] = new TH2F("h2MaxTrkPtvTowEt", "Event max(p_{T, track}) vs max(E_{T, tower})", 60, 0, 30, 80, 0, 40);

    histos1D["hTrackStats"] = new TH1F("hTrackStats", "Track Statistics", 10, -0.5, 9.5);
    histos1D["hTrackPt"]    = new TH1F("hTrackPt", "p_{T, track}", 60, 0.0, 30);
    histos1D["hTrackPtxCh"]    = new TH1F("hTrackPtxCh", "p_{T, track}#times Charge", 120, -30.0, 30.0);
    histos1D["hTrackEta"]   = new TH1F("hTrackEta", "#eta_{track}", 40, -1.0, 1.0);
    histos1D["hTrackPhi"]   = new TH1F("hTrackPhi", "#phi_{track}", 126, 0.0, 2*TMath::Pi());

    histos2D["h2TrackPtvEff"] = new TH2F("h2TrackPtvEff", "p_{T, track} vs #epsilon_{track}", 60, 0.0, 30, 20, 0.0, 1.0);
    histos2D["h2TrackEtavPhi"] = new TH2F("h2TrackEtavPhi", "#phi_{track} vs #eta_{track}", 126, 0.0, 2*TMath::Pi(), 40, -1.0, 1.0);

    histos1D["hTowerStats"]  = new TH1F("hTowerStats", "Tower Statistics", 10, -0.5, 9.5); 
    histos1D["hTowerERaw"]   = new TH1F("hTowerERaw", "E_{tower}, Uncorrected", 200, 0, 40);
    histos1D["hTowerE"]      = new TH1F("hTowerE", "E_{tower}, Corrected", 200, 0, 40);
    histos1D["hTowerEt"]     = new TH1F("hTowerEt", "E_{T, tower}", 200, 0, 40);
    histos1D["hTowerEta"]    = new TH1F("hTowerEta", "#eta_{tower}", 40, -1.0, 1.0);
    histos1D["hTowerPhi"]    = new TH1F("hTowerPhi", "#phi_{tower}", 126, 0.0, 2*TMath::Pi());

    histos2D["hTowerdESumvMax"] = new TH2F("hTowerdECorr", "#Delta E_{tower} Correction", 100, 0, 20, 100, 0, 20);
    histos2D["h2TowerEtavPhi"] = new TH2F("hTowerEtavPhi", "#phi_{tower} vs #eta_{tower}", 126, 0.0, 2*TMath::Pi(), 40, -1.0, 1.0);

    histos1D["hEventStats"]->GetXaxis()->SetBinLabel(1, "ALL");
    histos1D["hEventStats"]->GetXaxis()->SetBinLabel(2, "RUN GOOD");
    histos1D["hEventStats"]->GetXaxis()->SetBinLabel(3, "VZ PASS");
    histos1D["hEventStats"]->GetXaxis()->SetBinLabel(4, "HAS HT TRIGGER");
    histos1D["hEventStats"]->GetXaxis()->SetBinLabel(5, "CENTRALITY GOOD");
    histos1D["hEventStats"]->GetXaxis()->SetBinLabel(6, "CENTRALITY PASS");
    histos1D["hEventStats"]->GetXaxis()->SetBinLabel(7, "HAS JET");
    histos1D["hEventStats"]->GetXaxis()->SetBinLabel(8, "PT MAX PASS");
    histos1D["hEventStats"]->GetXaxis()->SetBinLabel(9, "GOOD");

    histos1D["hTriggerStats"]->GetXaxis()->SetBinLabel(1, "ALL");
    histos1D["hTriggerStats"]->GetXaxis()->SetBinLabel(2, "MBmon");
    histos1D["hTriggerStats"]->GetXaxis()->SetBinLabel(3, "MB5");
    histos1D["hTriggerStats"]->GetXaxis()->SetBinLabel(4, "MB30");
    histos1D["hTriggerStats"]->GetXaxis()->SetBinLabel(5, "HT1xMB30");
    histos1D["hTriggerStats"]->GetXaxis()->SetBinLabel(6, "HT2xMB30");
    histos1D["hTriggerStats"]->GetXaxis()->SetBinLabel(7, "HT3");

    histos1D["hTrackStats"]->GetXaxis()->SetBinLabel(1, "ALL");
    histos1D["hTrackStats"]->GetXaxis()->SetBinLabel(2, "DCA PASS");
    histos1D["hTrackStats"]->GetXaxis()->SetBinLabel(3, "nHitsFit PASS");
    histos1D["hTrackStats"]->GetXaxis()->SetBinLabel(4, "nHitsRatio PASS");
    histos1D["hTrackStats"]->GetXaxis()->SetBinLabel(5, "PT PASS");
    histos1D["hTrackStats"]->GetXaxis()->SetBinLabel(6, "ETA PASS");
    histos1D["hTrackStats"]->GetXaxis()->SetBinLabel(7, "GOOD");
    histos1D["hTrackStats"]->GetXaxis()->SetBinLabel(8, "TOWER MATCHED");
 
    histos1D["hTowerStats"]->GetXaxis()->SetBinLabel(1, "ALL");
    histos1D["hTowerStats"]->GetXaxis()->SetBinLabel(2, "GOOD");
    histos1D["hTowerStats"]->GetXaxis()->SetBinLabel(3, "ALIVE");
    histos1D["hTowerStats"]->GetXaxis()->SetBinLabel(4, "RawE PASS");
    histos1D["hTowerStats"]->GetXaxis()->SetBinLabel(5, "ETA PASS");
    histos1D["hTowerStats"]->GetXaxis()->SetBinLabel(6, "Et PASS");
    histos1D["hTowerStats"]->GetXaxis()->SetBinLabel(7, "ALL GOOD");
    histos1D["hTowerStats"]->GetXaxis()->SetBinLabel(8, "NO TRACKS MATCHED");

    for(const auto& hist : histos1D){
        hist.second->Sumw2();
    }

    for(const auto& hist : histos2D){
        hist.second->Sumw2();
    }

    if(doDebug)cout<<"StMyAnalysisMaker::initHistos() - DONE"<<endl;
        
}

//void StMyAnalysisMaker::setUpTriggers(){
//    if(runFlag == RunFlags::kRun14){
//        trigIds[Triggers::kDefault]     = {450005, 450008, 450009, 450010, 450014, 450015, 450018, 450020, 450024, 450025, 
//                                           450050, 450060, 450201, 450202, 450203, 450211, 450212, 450213 };               
//        trigIds[Triggers::kVPDMB5]      = {450005, 450008, 450009, 450014, 450015, 450018, 450024, 450025, 450050, 450060};
//        trigIds[Triggers::kVPDMB30]     = {450010, 450020};
//        trigIds[Triggers::kHT1xVPDMB30] = {450201, 450211};
//        trigIds[Triggers::kHT2xVPDMB30] = {450202, 450212};
//        trigIds[Triggers::kHT3]         = {450203, 450213};
//    }else if(runFlag == RunFlags::kRun12){
//        trigIds[Triggers::kDefault] = {370001, 370011, 370511, 370546, 390203, 370521, 370522, 370531, 370980, 380204, 
//                                       380205, 380205, 380208, 380206, 380216}; 
//        trigIds[Triggers::kVPDMB]   = {370001, 370011};
//        trigIds[Triggers::kHT1]     = {370511, 370546, 390203};
//        trigIds[Triggers::kHT2]     = {370521, 370522, 370531, 370980, 380204, 380205, 380205, 380208};
//        trigIds[Triggers::kHT3]     = {380206, 380216};
//    }
//
//}

double StMyAnalysisMaker::getTrackingEfficiency(double x, double y, int cbin, double zdcx, TFile *infile){
    double effBinContent = -99; // value to be extracted from histogram
    const char *species =  "pion"; // FIXME
    int lumiBin = floor(zdcx/10000);

    char* histName; 
    int effBin = -99;

    if(!doppAnalysis){
        if(x > 4.5) x = 4.5;  // for pt > 4.5 use value at 4.5
        // get 2D pTEta efficiency histo
        histName = Form("hTrack_%s_Efficiency_pTEta_final_centbin%d_lumibin%d", species, cbin, lumiBin);
        TH2F *h = static_cast<TH2F*>(infile->Get(Form("%s", histName)));
        if(!h) cout<<"don't have requested histogram! "<<Form("%s", histName)<<endl;
        h->SetName(Form("%s", histName));
        // get efficiency 
        effBin        = h->FindBin(x, y); // pt, eta
        effBinContent = h->GetBinContent(effBin);
        // delete histo and close input file
        delete h;
    }else{
       if(x > 1.8) x = 1.8;  // for pt > 1.8 use value at 1.8

       histName = Form("hppRun12_PtEtaEfficiency_data_aacuts");
       // changed from double to double
       TH2F *h = static_cast<TH2F*>(infile->Get(Form("%s", histName)));
       if(!h) cout<<"don't have requested histogram! "<<Form("%s", histName)<<endl;
       h->SetName(Form("%s", histName));
       // get efficiency 
       effBin        = h->FindBin(x, y); // pt, eta
       effBinContent = h->GetBinContent(effBin);
       double effBinContentErr = h->GetBinError(effBin);

       // delete histo and close input file
       delete h;
    }

    return effBinContent;
}

void StMyAnalysisMaker::setUpBadRuns(){
    string line;
    string inFileName;
    if(doppAnalysis){
        inFileName = "StRoot/StMyAnalysisMaker/runLists/Y2012_BadRuns_P12id_w_missing_HT.txt";
    }else{
        inFileName = "StRoot/StMyAnalysisMaker/runLists/Y2014_BadRuns_P18ih_w_missing_HT.txt";
    }
    ifstream inFile(inFileName.c_str());

    if( !inFile.good() ) {
        cout << "Can't open " << inFileName <<endl;;
    }else cout<<"Opening Bad Runs file: "<<inFileName<<endl;

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

void StMyAnalysisMaker::setUpBadTowers(){
    string inFileName;
    if(runFlag == RunFlags::kRun12){
        if(jetConstituentMinPt == 0.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2012_BadTowers_P12id.txt";
        else if(jetConstituentMinPt == 0.2)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2012_BadTowers_P12id_200MeV.txt";
        else if(jetConstituentMinPt == 1.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2012_BadTowers_P12id_1000MeV.txt";
        else if(jetConstituentMinPt == 2.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2012_BadTowers_P12id_2000MeV.txt";
    }else if(runFlag == RunFlags::kRun14){
        if(jetConstituentMinPt == 0.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2014_BadTowers_P18ih.txt";
        else if(jetConstituentMinPt == 0.2)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2014_BadTowers_P18ih_200MeV.txt";
        else if(jetConstituentMinPt == 1.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2014_BadTowers_P18ih_1000MeV.txt";
        else if(jetConstituentMinPt == 2.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2014_BadTowers_P18ih_2000MeV.txt";
    }

    string line;
    ifstream inFile (inFileName.c_str());
    if( !inFile.good() ) {
        cout << "Can't open " << inFileName <<endl;;
    }else cout<<"Opening Bad Tower file: "<<inFileName<<endl;

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

void StMyAnalysisMaker::setUpDeadTowers(){
    string inFileName;
    if(runFlag == RunFlags::kRun12) inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2012_DeadTowers.txt";
    else if(runFlag == RunFlags::kRun14) inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2014_DeadTowers_P18ih.txt";

     // open infile
    string line;
    ifstream inFile ( inFileName.c_str() );
    if( !inFile.good() ) {
        cout << "Can't open " << inFileName <<endl;;
    }else cout<<"Opening Dead Tower file: "<<inFileName<<endl;

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
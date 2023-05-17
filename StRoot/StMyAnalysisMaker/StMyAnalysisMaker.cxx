#define StMyAnalysisMaker_cxx

#include "StMyAnalysisMaker.h"

//ROOT includes
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
// centrality includes
#include "StRoot/StRefMultCorr/CentralityMaker.h"
#include "StRoot/StRefMultCorr/StRefMultCorr.h"
// STAR includes
#include "StRoot/StPicoEvent/StPicoDst.h"
#include "StRoot/StPicoDstMaker/StPicoDstMaker.h"
#include "StRoot/StPicoEvent/StPicoEvent.h"
#include "StRoot/StPicoEvent/StPicoTrack.h"
#include "StRoot/StPicoEvent/StPicoEmcTrigger.h"
#include "StRoot/StPicoEvent/StPicoBTowHit.h"
#include "StRoot/StPicoEvent/StPicoBEmcPidTraits.h"
//MyAnalysisMaker includes
#include "StEmcPosition2.h"
#include "StRoot/TStarEventClass/TStarEvent.h"
#include "StRoot/TStarEventClass/TStarTrack.h"
#include "StRoot/TStarEventClass/TStarTower.h"

ClassImp(StMyAnalysisMaker);

using namespace std;

StMyAnalysisMaker::StMyAnalysisMaker(string name, string output):
StMaker(name.c_str()){
    AnaName = name; 
    OutputFileName = output;

    TracksMatchedToTower.resize(4800);
}

StMyAnalysisMaker::~StMyAnalysisMaker(){
    if(EmcPosition) delete EmcPosition;
    if(EfficiencyFile) delete EfficiencyFile; 
    if(!_Event->TestBit(kIsOnHeap))delete _Event;
    if(hEventStats)delete hEventStats;
    if(hTrackStats)delete hTrackStats;
    if(hTowerStats)delete hTowerStats;
}

Int_t StMyAnalysisMaker::Init(){
    if(!doppAnalysis){
        grefmultCorr = CentralityMaker::instance()->getgRefMultCorr_P18ih_VpdMB30_AllLumi();
        grefmultCorrUtil = new StRefMultCorr("grefmult_P18ih_VpdMB30_AllLumi_MB5sc");
        grefmultCorrUtil->setVzForWeight(16, -16.0, 16.0);
        grefmultCorrUtil->readScaleForWeight("StRoot/StRefMultCorr/macros/weight_grefmult_vpd30_vpd5_Run14_P18ih_set1.txt");
    }

    if(!doRunbyRun)SetUpBadRuns(); //Probably deprecated, might add this into StRefMultCorr

    SetUpBadTowers();//There may be a better way
    SetUpDeadTowers();//There may be a better way
    DeclareHistograms();

    string EfficiencyFileName;

    if(doppAnalysis)EfficiencyFileName = "./StRoot/StMyAnalysisMaker/Run12_efficiency_New.root";
    else EfficiencyFileName = "./StRoot/StMyAnalysisMaker/Run14_efficiencySmaller2D.root";

    EfficiencyFile = new TFile(EfficiencyFileName.c_str(), "READ");
    
    EmcPosition = new StEmcPosition2();

    _Event = new TStarEvent();

    BookTree();

   // if(fout->IsOpen())   fout->Close();

    return kStOK;
}

void StMyAnalysisMaker::WriteHistograms(){
    hEventStats->Write();
    hTrackStats->Write();
    hTowerStats->Write();
}

Int_t StMyAnalysisMaker::Finish(){
    cout<< "StMyAnalysisMaker::Finish()"<<endl;

    if(OutputFileName != ""){
        fout->cd();
        WriteHistograms();
        fout->Write();
        fout->Close();
    }

    EfficiencyFile->Close();

    return kStOk;

}

Int_t StMyAnalysisMaker::Make(){
    picoDst_Maker = static_cast<StPicoDstMaker*>(GetMaker("picoDst"));
    if(!picoDst_Maker){
        cout<<"You havent added a StPicoDstMaker!"<<endl;
        return kStFatal;
    }

    picoDst = static_cast<StPicoDst*>(picoDst_Maker->picoDst());
    if(!picoDst){
        cout << " No PicoDst! Skip! " << endl;
        return kStWarn;
    }

    picoEvent = static_cast<StPicoEvent*>(picoDst->event());
    if(!picoEvent){
        cout<<" No PicoEvent! Skip! " << endl;
        return kStWarn;
    }

    RunID = picoEvent->runId();
    hEventStats->Fill(0);

    //Reject bad runs here..., if doing run by run jobs, reject bad runs while submitting jobs
    if(!doRunbyRun && badRuns.count(RunID)>0){
        hEventStats->Fill(1);
        return kStOK;
    }

    _Event->SetIdNumbers(RunID, picoEvent->eventId());
    pVtx = picoEvent->primaryVertex();
    _Event->SetPrimaryVertex(pVtx);
    //primary Z vertex cut...
    if(abs(_Event->Vz()) > AbsZVtx_Max){
        hEventStats->Fill(2);
        return kStOK;
    }
    //Min bias trigger related event stuff...
    EventTriggers = picoEvent->triggerIds();
    _Event->SetHT1Status(IsEventHT(HTTriggers::kHT1));
    _Event->SetHT2Status(IsEventHT(HTTriggers::kHT2));
    _Event->SetHT3Status(IsEventHT(HTTriggers::kHT3));

    if(!(_Event->IsHT())){
        hEventStats->Fill(3);
        if(doHTEventsOnly)return kStOK;
    }

    _Event->SetMBStatus(IsEventMB(MBTriggers::kVPDMB)); 
    _Event->SetMB5Status(IsEventMB(MBTriggers::kVPDMB5)); 
    _Event->SetMB30Status(IsEventMB(MBTriggers::kVPDMB30));

    if(!(_Event->IsMB())){
        hEventStats->Fill(4);
        if(doMBEventsOnly)return kStOK;
    }

    _Event->SetRefMults(picoEvent->grefMult(), picoEvent->refMult()); 
    _Event->SetZDCCoincidence(picoEvent->ZDCx());

    if(!doppAnalysis && !grefmultCorr){
        cout<<"WARNING: Doing heavy-ion analysis without StRefMultCorr"<<endl;
    }else if(!doppAnalysis){
        grefmultCorr->init(RunID);
        grefmultCorr->initEvent(_Event->gRefMult(), _Event->Vz(), _Event->ZDC_Coincidence());
        //centbin9 = grefmultCorr->getCentralityBin9();
        centbin16 = grefmultCorr->getCentralityBin16();
        //ref9 = 8-centbin9;
        if(centbin16 < 0){
            hEventStats->Fill(5);
            return kStOK;
        }
        ref16 = 15-centbin16; 
        _Event->SetCentrality(5.0*ref16);
        if((_Event->Centrality() < CentralityMin) || (_Event->Centrality() > CentralityMax)){
            hEventStats->Fill(6);
            return kStOK;
        }

        _Event->SetCorrectedRefmult(grefmultCorr->getRefMultCorr(_Event->gRefMult(), _Event->Vz(), _Event->ZDC_Coincidence(), 2));
        _Event->SetWeight(grefmultCorr->getWeight());
        //grefmultCorrUtil->init(RunID);
        //grefmultCorrUtil->initEvent(_Event->gRefMult(), _Event->Vz(), _Event->ZDC_Coincidence()); 
        //_Event->SetMB5toMB30Reweight((_Event->IsMB5() && !_Event->IsMB30()) ? grefmultCorrUtil->getWeight() : 1.0);
    }
        
    _Event->SetBBCCoincidence(picoEvent->BBCx());
    _Event->SetVPDVz(picoEvent->vzVpd());

    hEventStats->Fill(7);

    //RunOverEmcTriggers(); //Runs over all emc i.e., High tower (and Jet patch for pp) triggers... 
    _Event->ClearTrackArray();
    RunOverTracks(); //Runs over all tracks
    _Event->ClearTowerArray();
    RunOverTowers(); //Runs over all towers

    tree->Fill();

    return kStOk;
}

void StMyAnalysisMaker::BookTree(){
    if(OutputFileName == ""){
        cout<<"Trees are not being written to any file!"<<endl;
        tree = new TTree("Event_Info", "Tree with event Info");
        tree->Branch("Events", &(_Event)); 
    }else{
        fout = new TFile(OutputFileName.c_str(), "UPDATE");
        fout->cd();
        //fout->mkdir(GetName());
        //writedir = (TDirectory*)fout->Get(GetName());
        cout<<"Writing tree to: "<<OutputFileName<<endl;
        tree = new TTree("Event_Info", "Tree with event Info");
        tree->SetDirectory(gDirectory);
        tree->Branch("Events", &(_Event), 32000, 99); 
        cout<<"Events tree directory set"<<endl;
    }
}

//void StMyAnalysisMaker::RunOverEmcTriggers(){
//    //_Event->SetNumberOfEmcTriggers(picoDst->numberOfEmcTriggers());
//    //empty and initialize HighTowerStatus arrays for all 4800 towers 
//    HighTowerStatus.assign(4800, {false, false, false, false});
//
//    for(int itrig = 0; itrig < picoDst->numberOfEmcTriggers(); itrig++){
//        StPicoEmcTrigger *trigger = static_cast<StPicoEmcTrigger*>(picoDst->emcTrigger(itrig));
//        int itow = trigger->id() - 1;
//        if(trigger->isHT0())      {HighTowerStatus[itow][0] = true;}
//        else if(trigger->isHT1()) {HighTowerStatus[itow][1] = true;}
//        else if(trigger->isHT2()) {HighTowerStatus[itow][2] = true;}
//        else if(trigger->isHT3()) {HighTowerStatus[itow][3] = true;}
//    }
//}

void StMyAnalysisMaker::RunOverTracks(){
    _Event->SetNumberOfGlobalTracks(picoDst->numberOfTracks());

    TracksMatchedToTower.assign(4800, {}); 
    TrackPtMax = 0;

    if(_Event->Tracks->GetEntriesFast() > 0){
        cout<<"Track array not cleared from previous event!"<<endl;
    }

    for(int itrk = 0; itrk < picoDst->numberOfTracks(); itrk++){ //begin Track Loop...
        StPicoTrack *trk = static_cast<StPicoTrack*>(picoDst->track(itrk));
        if(!(trk->isPrimary())) continue; //Check if track is primary
        hTrackStats->Fill(0);
        //Track quality cuts...
        if(trk->gDCA(pVtx).Mag() > TrackDCAMax){hTrackStats->Fill(1); continue;}
        if(trk->nHitsFit() < TrackNHitsFitMin){hTrackStats->Fill(2); continue;} 
        if(double(trk->nHitsFit()/trk->nHitsMax()) < TrackNHitsRatioMin){
            hTrackStats->Fill(3); 
            continue;
        }

        TVector3 trkMom = trk->pMom();
        double pt = trkMom.Pt();
        double eta = trkMom.Eta();

        if(pt < TrackPtMin){hTrackStats->Fill(4); continue;} 
        if((eta < TrackEtaMin) || (eta > TrackEtaMax)){hTrackStats->Fill(5); continue;}

        if(pt > TrackPtMax) TrackPtMax = pt; 

        double trackingEff = GetTrackingEfficiency(pt, eta, ref16, _Event->ZDC_Coincidence(), EfficiencyFile);

        int MatchedTowerIndex = trk->bemcTowerIndex();
        if(MatchedTowerIndex >= 0){
            TracksMatchedToTower[MatchedTowerIndex].push_back(itrk);
            hTrackStats->Fill(6);
        }

        hTrackStats->Fill(7);

        TStarTrack *_track = _Event->AddTrack();
        _track->SetIndex(itrk);
        _track->SetCharge(trk->charge());
        _track->SetPxPyPz(trkMom);
        _track->SetTrackingEfficiency(trackingEff);
        _track->SetMatchedTower(MatchedTowerIndex); 
        _track->DoTrackPid(trk->nSigmaPion(), trk->nSigmaKaon(), 
                           trk->nSigmaProton(), trk->nSigmaElectron());       

        //if(!doJetAnalysis) continue;
        //if(_track.Pt < JetConstituentMinPt)continue;
        ////Put code to make precursor of vector<PseudoJet> here...
        //double E = sqrt(trkMom.Mag2()+pi0mass*pi0mass);
        //TracksForJetClustering[itrk] = make_pair(new TLorentzVector(trkMom, E), _track.Charge);   
   }
   _Event->SetMaxTrackPt(TrackPtMax);
}

void StMyAnalysisMaker::RunOverTowers(){    

    TowerEtMax = 0;

    if(_Event->Towers->GetEntriesFast() > 0){
        cout<<"Tower array not cleared from previous event!"<<endl;
    }
    for(int itow = 0; itow < picoDst->numberOfBTowHits(); itow++){
        StPicoBTowHit *tower = static_cast<StPicoBTowHit*>(picoDst->btowHit(itow));
        hTowerStats->Fill(0);
        if(badTowers.count(itow+1)>0){hTowerStats->Fill(1); continue;}
        if(deadTowers.count(itow+1)>0){hTowerStats->Fill(2); continue;}
        if(tower->energy() < TowerEnergyMin){hTowerStats->Fill(3); continue;}

        //Get tower's position...
        TVector3 towPos = EmcPosition->getPosFromVertex(pVtx, itow+1);
        if((towPos.Eta() < TowerEtaMin) || (towPos.Eta() > TowerEtaMax)){
            hTowerStats->Fill(4);
            continue;
        }  

        //Start hardonic correction of tower...
        double EnergyCorr = tower->energy();

        //if(!(TracksMatchedToTower[itow].empty()))
        //cout<<"Tower: "<<itow<<" "<<Eta<<" "<<Phi<<" "<<" "<<EnergyCorr<<" "<<TracksMatchedToTower[itow].size()<<endl;
        if((TypeOfHadCorr != HadronicCorrectionType::kNone) && !(TracksMatchedToTower[itow].empty())){
            //cout<<EnergyCorr<<" ";
            double maxE = 0;
            double sumE = 0;
            for(int itrk : TracksMatchedToTower[itow]){ 
               //cout<<itrk<<endl;
               StPicoTrack *trk = static_cast<StPicoTrack*>(picoDst->track(itrk));
               TVector3 trkMom = trk->pMom();
               double p2 = trkMom.Mag2();
               double E = sqrt(p2 + pi0mass*pi0mass);
               if(E > maxE) maxE = E;
               sumE += E;
            }
            //cout<<sumE<<" "<<maxE<<" ";
            if(TypeOfHadCorr == HadronicCorrectionType::kFull)
                EnergyCorr -= sumE;
            else if(TypeOfHadCorr == HadronicCorrectionType::kHighestMatchedTrackE) 
                EnergyCorr -= maxE;
            //cout<<EnergyCorr<<endl;
        }else{hTowerStats->Fill(5);}

        if(EnergyCorr < TowerEnergyMin){hTowerStats->Fill(6); continue;}
        double towEt = EnergyCorr/cosh(towPos.Eta());
        if(towEt < TowerEnergyMin){hTowerStats->Fill(7); continue;}

        hTowerStats->Fill(8);

        TStarTower *_tower = _Event->AddTower();
        _tower->SetIndex(itow);
        _tower->SetADC(tower->adc());
        _tower->SetE(EnergyCorr);
        _tower->SetRawE(tower->energy());
        _tower->SetPosXYZ(towPos);
        _tower->SetNMatchedTracks(TracksMatchedToTower[itow].size());

        if(towEt > TowerEtMax)TowerEtMax = towEt;

        //cout<<"_____T0: "<<_tower.IsHT0<<" T1: "<<_tower.IsHT1<<" T2: "<<_tower.IsHT2<<" T3: "<<_tower.IsHT3<<" E: "<<_tower.Energy<<endl;

        //if(!doJetAnalysis || !doFullJet) continue;
        //if(towEt < JetConstituentMinPt) continue;
        ////Put code to fill precursor of vector<PseudoJet> if doing full jets here...
        //double towP = sqrt(EnergyCorr*EnergyCorr-pi0mass*pi0mass); 
        //towPos.SetMag(towP);
        //TowersForJetClustering[itow] = new TLorentzVector(towPos, EnergyCorr);
    }

    _Event->SetMaxTowerEt(TowerEtMax);

}

void StMyAnalysisMaker::RunOverTowerClusters(){
//Will put methods relating to analyzing objects from StPicoBEmcPidTraits...

}

void StMyAnalysisMaker::DeclareHistograms(){
    hEventStats = new TH1F("hEventStats", "Event Statistics", 9, -0.5, 8.5);
    hEventStats->Sumw2();
    hEventStats->GetXaxis()->SetBinLabel(1, "ALL");
    hEventStats->GetXaxis()->SetBinLabel(2, "Bad runs");
    hEventStats->GetXaxis()->SetBinLabel(3, Form("|V_{z}| < %0.2f", AbsZVtx_Max));
    hEventStats->GetXaxis()->SetBinLabel(4, "no HT");
    hEventStats->GetXaxis()->SetBinLabel(5, "not MB");
    hEventStats->GetXaxis()->SetBinLabel(6, "Centrality > 80");
    hEventStats->GetXaxis()->SetBinLabel(7, Form("%0.0f > Centrality > %0.2f", CentralityMin, CentralityMax));
    hEventStats->GetXaxis()->SetBinLabel(8, "GOOD");

    hTrackStats = new TH1F("hTrackStats", "Track Statistics", 9, -0.5, 8.5);
    hTrackStats->Sumw2();
    hTrackStats->GetXaxis()->SetBinLabel(1, "ALL");
    hTrackStats->GetXaxis()->SetBinLabel(2, Form("DCA > %0.2f", TrackDCAMax));
    hTrackStats->GetXaxis()->SetBinLabel(3, Form("nHitsFit > %0.2f", TrackNHitsFitMin));
    hTrackStats->GetXaxis()->SetBinLabel(4, Form("nHits(Fit/Max) > %0.2f", TrackNHitsRatioMin));
    hTrackStats->GetXaxis()->SetBinLabel(5, Form("p_{T} < %0.2f", TrackPtMin));
    hTrackStats->GetXaxis()->SetBinLabel(6, Form("|#eta| > %0.2f", TrackEtaMax));
    hTrackStats->GetXaxis()->SetBinLabel(7, "No tower match");
    hTrackStats->GetXaxis()->SetBinLabel(8, "GOOD");

    hTowerStats = new TH1F("hTowerStats", "Tower Statistics", 9, -0.5, 8.5);
    hTowerStats->Sumw2();
    hTowerStats->GetXaxis()->SetBinLabel(1, "ALL");
    hTowerStats->GetXaxis()->SetBinLabel(2, "Bad");
    hTowerStats->GetXaxis()->SetBinLabel(3, "Dead");
    hTowerStats->GetXaxis()->SetBinLabel(4, Form("RawE < %0.2f", TowerEnergyMin));
    hTowerStats->GetXaxis()->SetBinLabel(5, Form("|#eta| > %0.2f", TowerEtaMax));
    hTowerStats->GetXaxis()->SetBinLabel(6, "No matched tracks");
    hTowerStats->GetXaxis()->SetBinLabel(7, Form("E < %0.2f", TowerEnergyMin));
    hTowerStats->GetXaxis()->SetBinLabel(8, Form("E_{T} < %0.2f", TowerEnergyMin));
    hTowerStats->GetXaxis()->SetBinLabel(9, "GOOD");
        
}

bool StMyAnalysisMaker::IsEventMB(MBTriggers type){
    vector<unsigned int> TriggerIds;
    switch(Run_Flag){
        case RunFlags::kRun12 : 
            switch(type){
                case MBTriggers::kVPDMB : TriggerIds = {370011};break; //Why not 370001 ?
                case MBTriggers::kVPDMB_extra : TriggerIds = {370001, 370011};break;
                case MBTriggers::kVPDMB5 : TriggerIds = {0};break; //These dont exist for Run12 (pp)
                case MBTriggers::kVPDMB30 : TriggerIds = {0};break;
                case MBTriggers::kVPDMB30_extra : TriggerIds = {0};break;
                default : TriggerIds = {370011}; 
            }break;
        case RunFlags::kRun14 :
            switch(type){
                case MBTriggers::kVPDMB : TriggerIds = {450014};break; //Why not 450001 ?
                case MBTriggers::kVPDMB_extra : TriggerIds = {440001, 450014};break;
                case MBTriggers::kVPDMB5 : TriggerIds = {450005, 450008, 450009, 450014, 450015, 450018, 450024, 450025, 450050, 450060};break;
                case MBTriggers::kVPDMB30 : TriggerIds = {450010, 450020};break;
                case MBTriggers::kVPDMB30_extra : TriggerIds = {450010, 450020, 450201, 450202, 450211, 450212};break;
                default : TriggerIds = {450014}; 
            }break;   
    }

    return find_first_of(EventTriggers.begin(), EventTriggers.end(), TriggerIds.begin(), TriggerIds.end()) != EventTriggers.end();
}

bool StMyAnalysisMaker::IsEventHT(HTTriggers type){
   vector<unsigned int> TriggerIds;
    switch(Run_Flag){
        case RunFlags::kRun12 : 
            switch(type){
                case HTTriggers::kHT1 : TriggerIds = {370511, 370546};break; //Why not 370001 ?
                case HTTriggers::kHT2 : TriggerIds = {370521, 370522, 370531, 370980};break; //Why not 370001 ?
                case HTTriggers::kHT3 : TriggerIds = {380206, 380216};break; //Why not 370001 ?
                default : TriggerIds = {370521, 370522, 370531, 370980}; 
            }break;
        case RunFlags::kRun14 :
            switch(type){
                case HTTriggers::kHT1 : TriggerIds = {450201, 450211, 460201};break; //Why not 460001 ?
                case HTTriggers::kHT2 : TriggerIds = {450202, 450212, 460202, 460212};break;
                case HTTriggers::kHT3 : TriggerIds = {450203, 450213, 460203};break;
                default : TriggerIds = {450202, 450212, 460202, 460212}; 
            }break;   
    }

    return find_first_of(EventTriggers.begin(), EventTriggers.end(), TriggerIds.begin(), TriggerIds.end()) != EventTriggers.end(); 
}

double StMyAnalysisMaker::GetTrackingEfficiency(double x, double y, int cbin, double zdcx, TFile *infile){
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
       // changed from double to float
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

void StMyAnalysisMaker::SetUpBadRuns(){
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

void StMyAnalysisMaker::SetUpBadTowers(){
    string inFileName;
    if(Run_Flag == RunFlags::kRun12){
        if(JetConstituentMinPt == 0.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2012_BadTowers_P12id.txt";
        else if(JetConstituentMinPt == 0.2)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2012_BadTowers_P12id_200MeV.txt";
        else if(JetConstituentMinPt == 1.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2012_BadTowers_P12id_1000MeV.txt";
        else if(JetConstituentMinPt == 2.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2012_BadTowers_P12id_2000MeV.txt";
    }else if(Run_Flag == RunFlags::kRun14){
        if(JetConstituentMinPt == 0.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2014_BadTowers_P18ih.txt";
        else if(JetConstituentMinPt == 0.2)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2014_BadTowers_P18ih_200MeV.txt";
        else if(JetConstituentMinPt == 1.0)
            inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2014_BadTowers_P18ih_1000MeV.txt";
        else if(JetConstituentMinPt == 2.0)
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

void StMyAnalysisMaker::SetUpDeadTowers(){
    string inFileName;
    if(Run_Flag == RunFlags::kRun12) inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2012_DeadTowers.txt";
    else if(Run_Flag == RunFlags::kRun14) inFileName = "StRoot/StMyAnalysisMaker/towerLists/Y2014_DeadTowers_P18ih.txt";

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

#include "StMyAnalysisMaker.h"

//ROOT includes
#include "TH2.h"
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
#include "StEventTreeStruct.h"

ClassImp(StMyAnalysisMaker);

StMyAnalysisMaker::StMyAnalysisMaker(string name, string output):
StMaker(name.c_str()){
    AnaName = name; 
    OutputFileName = output;

    TracksMatchedToTower.resize(4800);
    HighTowerStatus.assign(4800, {false, false, false, false});
}

StMyAnalysisMaker::~StMyAnalysisMaker(){
    if(EmcPosition) delete EmcPosition;
    if(EfficiencyFile) delete EfficiencyFile; 
}

Int_t StMyAnalysisMaker::Init(){
    if(!doppAnalysis){
        grefmultCorr = CentralityMaker::instance()->getgRefMultCorr_P18ih_VpdMB30_AllLumi();
        grefmultCorrUtil = new StRefMultCorr("grefmult_P18ih_VpdMB30_AllLumi_MB5sc");
        grefmultCorrUtil->setVzForWeight(16, -16.0, 16.0);
        grefmultCorrUtil->readScaleForWeight("StRoot/StRefMultCorr/macros/weight_grefmult_vpd30_vpd5_Run14_P18ih_set1.txt");
    }

    SetUpBadRuns(); //Probably deprecated, might add this into StRefMultCorr

    SetUpBadTowers();//There may be a better way
    SetUpDeadTowers();//There may be a better way

    string EfficiencyFileName;

    if(doppAnalysis)EfficiencyFileName = "./StRoot/StMyAnalysisMaker/Run12_efficiency_New.root";
    else EfficiencyFileName = "./StRoot/StMyAnalysisMaker/Run14_efficiencySmaller2D.root";

    EfficiencyFile = new TFile(EfficiencyFileName.c_str(), "READ");
    
    EmcPosition = new StEmcPosition2();

    _Event = new MyStEvent();

    BookTree();

   // if(fout->IsOpen())   fout->Close();

    return kStOK;
}

Int_t StMyAnalysisMaker::Finish(){
    cout<< "StMyAnalysisMaker::Finish()"<<endl;

    if(OutputFileName != ""){
        fout->cd();
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
    EventID = picoEvent->eventId();

    //Reject bad runs here...,
    if(badRuns.count(RunID)>0)return kStOK;

    pVtx = picoEvent->primaryVertex();
    double pVtx_Z = pVtx.z();
    //primary Z vertex cut...
    if((pVtx_Z < ZVtx_Min)||(pVtx_Z > ZVtx_Max)) return kStOK;

    int gRefMult = picoEvent->grefMult();
    int RefMult = picoEvent->refMult();
    double BBCxx = picoEvent->BBCx();
    double ZDCxx = picoEvent->ZDCx();

    //Min bias trigger related event stuff...
    EventTriggers = picoEvent->triggerIds();
    bool IsMB = IsEventMB(MBTriggers::kVPDMB);

    double RefMultCorr = RefMult;
    bool IsMB5 = false;
    bool IsMB30 = false;
    double Peripheral_ReWeight = 1;
    double MB5toMB30_ReWeight = 0;
    if(!doppAnalysis){ 
        IsMB5  = IsEventMB(MBTriggers::kVPDMB5); 
        IsMB30 = IsEventMB(MBTriggers::kVPDMB30); 
        //Centrality related stuff...
        grefmultCorr->init(RunID);
        grefmultCorr->initEvent(gRefMult, pVtx_Z, ZDCxx);
        RefMultCorr = grefmultCorr->getRefMultCorr(gRefMult, pVtx_Z, ZDCxx, 2);
        centbin9 = grefmultCorr->getCentralityBin9();
        centbin16 = grefmultCorr->getCentralityBin16();
        ref9 = 8-centbin9;
        ref16 = 15-centbin16; 
        Peripheral_ReWeight = grefmultCorr->getWeight();

        grefmultCorrUtil->init(RunID);
        grefmultCorrUtil->initEvent(gRefMult, pVtx_Z, ZDCxx); 
        MB5toMB30_ReWeight = (IsMB5 && !IsMB30) ? grefmultCorrUtil->getWeight() : 1.0;

        if(centbin16 < 0) return kStOK;
    }

    bool IsEventMB = false;
    if(IsMB)IsEventMB = true;
    if(IsMB5)IsEventMB = true;
    if(IsMB30)IsEventMB = true;
    
    _Event->RunID = RunID;
    _Event->EventID = EventID;
    _Event->gRefMult = gRefMult;
    _Event->RefMult = RefMult;
    _Event->RefMultCorr = RefMultCorr;
    _Event->Centrality = 5.0*ref16;
    _Event->Peripheral_ReWeight = Peripheral_ReWeight;
    _Event->MB5toMB30_ReWeight = MB5toMB30_ReWeight;
    _Event->IsMB = IsMB;
    _Event->IsMB5 = IsMB5;
    _Event->IsMB30 = IsMB30;
    _Event->pVtx_Z = pVtx_Z;
    _Event->pVtx_r = pVtx.Perp();
    _Event->VPD_Vz = picoEvent->vzVpd();
    _Event->ZDCxx = ZDCxx;
    _Event->BBCxx = BBCxx;

    _Event->ClearVectors();

    RunOverEmcTriggers(); //Runs over all emc i.e., High tower (and Jet patch for pp) triggers...
    bool isHT = false;
    if(_Event->IsHT0)isHT = true;
    if(_Event->IsHT1)isHT = true;
    if(_Event->IsHT2)isHT = true;
    if(_Event->IsHT3)isHT = true;

    if(!IsEventMB && !isHT)return kStOK; //Require atleast one trigger to be fired...

    if(isHT) cout<<"HT!"<<endl;
    //if((!_Event->IsHT0 && !_Event->IsHT1)&&(!_Event->IsHT2 && !_Event->IsHT3));
    RunOverTracks(); //Runs over all tracks
    RunOverTowers(); //Runs over all towers

    tree->Fill();

    return kStOk;

}

void StMyAnalysisMaker::BookTree(){
    if(OutputFileName == ""){
        cout<<"Trees are not being written to any file!"<<endl;
        tree = new TTree("Event_Info", "Tree with event Info");
        tree->Branch("Event", "MyStEvent", &(_Event)); 
    }else{
        fout = new TFile(OutputFileName.c_str(), "UPDATE");
        fout->cd();
        //fout->mkdir(GetName());
        //writedir = (TDirectory*)fout->Get(GetName());
        cout<<"Writing tree to: "<<OutputFileName<<endl;
        tree = new TTree("Event_Info", "Tree with event Info");
        tree->SetDirectory(gDirectory);
        tree->Branch("Event", &(_Event), 32000, 99); 
        cout<<"Events tree directory set"<<endl;
    }
}

void StMyAnalysisMaker::RunOverEmcTriggers(){
    _Event->NTriggers = picoDst->numberOfEmcTriggers();

    //empty and initialize HighTowerStatusArrays for all 4800 towers 
    HighTowerStatus.assign(4800, {false, false, false, false});

    for(int itrig = 0; itrig < picoDst->numberOfEmcTriggers(); itrig++){
        StPicoEmcTrigger *trigger = static_cast<StPicoEmcTrigger*>(picoDst->emcTrigger(itrig));
        int itow = trigger->id() - 1;
        if(trigger->isHT0())     { HighTowerStatus[itow][0] = true; _Event->IsHT0 = true; continue;}
        else if(trigger->isHT1()){ HighTowerStatus[itow][1] = true; _Event->IsHT1 = true; continue;}
        else if(trigger->isHT2()){ HighTowerStatus[itow][2] = true; _Event->IsHT2 = true; continue;}
        else if(trigger->isHT3()){ HighTowerStatus[itow][3] = true; _Event->IsHT3 = true; continue;}
    }
}

void StMyAnalysisMaker::RunOverTracks(){
    _Event->NTracks = picoDst->numberOfTracks();
    //cout<<"# primary tracks in event: "<<_Event->NTracks<<" "<<picoEvent->numberOfPrimaryTracks()<<endl;
    TracksMatchedToTower.assign(4800, {}); 
    TrackPtMax = 0;

    for(int itrk = 0; itrk < picoDst->numberOfTracks(); itrk++){ //begin Track Loop...
        StPicoTrack *trk = static_cast<StPicoTrack*>(picoDst->track(itrk));
        if(!(trk->isPrimary())) continue; //Check if track is primary
        //Track quality cuts...
        if(trk->gDCA(pVtx).Mag() > TrackDCAMax) continue;
        if(trk->nHitsFit() < TrackNHitsFitMin) continue;
        if(double(trk->nHitsFit()/trk->nHitsMax()) < TrackNHitsRatioMin) continue;

        TVector3 trkMom = trk->pMom();

        if(trkMom.Pt() < TrackPtMin) continue; 
        //if(_Tracks[itrk].Pt > TrackPtMax) continue;
        if((trkMom.Eta() < TrackEtaMin) || (trkMom.Eta() > TrackEtaMax)) continue;

        double trackingEff = GetTrackingEfficiency(trkMom, ref16, _Event->ZDCxx, EfficiencyFile);

        int MatchedTowerIndex = trk->bemcTowerIndex();
        if(MatchedTowerIndex >= 0){
            TracksMatchedToTower[MatchedTowerIndex].push_back(itrk);
            //cout<<"Track: "<<itrk<<" Matched Tower: "<<MatchedTowerIndex<<endl;
        }

        MyStTrack _track(itrk, trkMom, trk->charge());
        _track.trackingEff = trackingEff;
        _track.MatchedTowerIndex = MatchedTowerIndex;

        _Event->AddTrack(_track);
        if(_track.Pt > TrackPtMax) TrackPtMax = _track.Pt; 

        //if(!doJetAnalysis) continue;
        //if(_track.Pt < JetConstituentMinPt)continue;
        ////Put code to make precursor of vector<PseudoJet> here...
        //double E = sqrt(trkMom.Mag2()+pi0mass*pi0mass);
        //TracksForJetClustering[itrk] = make_pair(new TLorentzVector(trkMom, E), _track.Charge);   
   }
   _Event->MaxTrackPt = TrackPtMax;

}

void StMyAnalysisMaker::RunOverTowers(){    
    _Event->NTowers = picoDst->numberOfBTowHits();

    TowerEtMax = 0;

    for(int itow = 0; itow < picoDst->numberOfBTowHits(); itow++){
        StPicoBTowHit *tower = static_cast<StPicoBTowHit*>(picoDst->btowHit(itow));
        if(badTowers.count(itow+1)>0)continue;
        if(deadTowers.count(itow+1)>0)continue;
        if(tower->energy() < TowerEnergyMin)continue;

        //Get tower's position...
        TVector3 towPos = EmcPosition->getPosFromVertex(pVtx, itow+1);
        if((towPos.Eta() < TowerEtaMin) || (towPos.Eta() > TowerEtaMax)) continue;  

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
        }

        MyStTower _tower(itow, tower->adc(), tower->energy(), towPos);
        _tower.SetHighTowerStatus(HighTowerStatus[itow]);
        _tower.NMatchedTracks = TracksMatchedToTower[itow].size();
        _tower.EnergyCorr = EnergyCorr;

        _Event->AddTower(_tower);

        if(EnergyCorr < TowerEnergyMin) continue;
        double towEt = EnergyCorr/cosh(towPos.Eta());
        if(towEt < TowerEnergyMin) continue;

        if(towEt > TowerEtMax)TowerEtMax = towEt;

        //cout<<"_____T0: "<<_tower.IsHT0<<" T1: "<<_tower.IsHT1<<" T2: "<<_tower.IsHT2<<" T3: "<<_tower.IsHT3<<" E: "<<_tower.Energy<<endl;

        //if(!doJetAnalysis || !doFullJet) continue;
        //if(towEt < JetConstituentMinPt) continue;
        ////Put code to fill precursor of vector<PseudoJet> if doing full jets here...
        //double towP = sqrt(EnergyCorr*EnergyCorr-pi0mass*pi0mass); 
        //towPos.SetMag(towP);
        //TowersForJetClustering[itow] = new TLorentzVector(towPos, EnergyCorr);
    }

    _Event->MaxTowerEt = TowerEtMax;

}

void StMyAnalysisMaker::RunOverTowerClusters(){
//Will put methods relating to analyzing objects from StPicoBEmcPidTraits...

}

bool StMyAnalysisMaker::IsEventMB(MBTriggers type){
    vector<unsigned int> TriggerIds;
    switch(Run_Flag){
        case RunFlags::kRun12 : 
            switch(type){
                case MBTriggers::kVPDMB : TriggerIds = {370011};break; //Why not 370001 ?
                case MBTriggers::kVPDMB_extra : TriggerIds = {370001, 370011};break;
                default : TriggerIds = {370011}; 
            }break;
        case RunFlags::kRun14 :
            switch(type){
                case MBTriggers::kVPDMB : TriggerIds = {450014};break; //Why not 450001 ?
                case MBTriggers::kVPDMB_extra : TriggerIds = {450001, 450014};break;
                case MBTriggers::kVPDMB5 : TriggerIds = {450005, 450008, 450009, 450014, 450015, 450018, 450024, 450025, 450050, 450060};break;
                case MBTriggers::kVPDMB30 : TriggerIds = {450010, 450020};break;
                case MBTriggers::kVPDMB30_extra : TriggerIds = {450010, 450020, 450201, 450202, 450211, 450212};break;
                default : TriggerIds = {450014}; 
            }break;   
    }

    return find_first_of(EventTriggers.begin(), EventTriggers.end(), TriggerIds.begin(), TriggerIds.end()) != EventTriggers.end();
}

double StMyAnalysisMaker::GetTrackingEfficiency(TVector3& tMom, int cbin, double zdcx, TFile *infile){
    double x = tMom.Pt();
    double y = tMom.Eta();

    double effBinContent = -99; // value extracted from histogram
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

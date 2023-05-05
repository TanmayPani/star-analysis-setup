#include "StMyJetMaker.h"
//FastJet stuff
#include "FJ_includes.h"
//My StRoot includes
#include "StRoot/StMyAnalysisMaker/StMyAnalysisMaker.h"
#include "StRoot/TStarEventClass/TStarEvent.h"
#include "StJetTreeStruct.h"

using namespace fastjet;

ClassImp(StMyJetMaker);

StMyJetMaker::StMyJetMaker(string name, string analysis, string output): 
StMaker(name.c_str()){
    Analysis = analysis;
    OutputFileName = output;    
}

StMyJetMaker::~StMyJetMaker(){

}

Int_t StMyJetMaker::Init(){

    _JetEvent = new StJetEvent();
     
    BookTree();

    return kStOK;
}

Int_t StMyJetMaker::Finish(){
    cout<< "StMyJetMaker::Finish()"<<endl;
    if(OutputFileName != ""){
        fout->cd();
        fout->Write();
        fout->Close();
    }
    return kStOK;
}

Int_t StMyJetMaker::Make(){

    anaMaker = static_cast<StMyAnalysisMaker*>(GetMaker(Analysis.c_str()));
   
    myEvent = static_cast<TStarEvent*>(anaMaker->GetEvent()); 

    //if(!myEvent->IsHT2)return kStOK;

    _JetEvent->RunID = myEvent->RunNumber();
    _JetEvent->EventID = myEvent->EventNumber();

    vector<PseudoJet> *jet_constituents;

    _Tracks = static_cast<TClonesArray*>(myEvent->GetTracks());

    for(int itrk = 0; itrk < _Tracks->GetEntriesFast(); itrk++){
        TStarTrack *_track = static_cast<TStarTrack*>(_Tracks->At(itrk)); 
        if(_track->Pt() < JetConstituentMinPt)continue;  
        PseudoJet constit(_track->Px(), _track->Py(), _track->Pz(), _track->Pi0E());
        constit.set_user_info(new UserInfo(_track->Index(), _track->Charge()));
        jet_constituents->push_back(move(constit));
    }

    _Towers = static_cast<TClonesArray*>(myEvent->GetTowers());

    for(int itow = 0; itow < _Towers->GetEntriesFast(); itow++){
        TStarTower *_tower = static_cast<TStarTower*>(_Towers->At(itow));
        if(_tower->Pt() < JetConstituentMinPt)continue;
        PseudoJet constit(_tower->Px(), _tower->Py(), _tower->Pz(), _tower->E());
        constit.set_user_info(new UserInfo(_tower->Index(), 0));
        jet_constituents->push_back(move(constit));
    } 
   
    JetDefinition *jet_def = nullptr;
    JetDefinition *bkg_jet_def = nullptr;

    GhostedAreaSpec *area_spec = nullptr;
    AreaDefinition *area_def = nullptr;

    ClusterSequence *CS = nullptr;
    ClusterSequenceArea *CS_Area = nullptr;
    Selector bkg_selector = SelectorAbsRapMax(3.0)*(!SelectorNHardest(2));
    JetMedianBackgroundEstimator* mBGE = nullptr; 

    Selector JetCuts = SelectorPtMin(JetPtMin)*SelectorAbsEtaMax(JetAbsEtaMax);
    Selector no_ghost = !SelectorIsPureGhost();
    vector<PseudoJet> jets;

    jet_def = new JetDefinition(antikt_algorithm, R, BIpt2_scheme, Best);
    if(doBackgroundCalc){
        area_spec = new GhostedAreaSpec(MaxRap);
        area_def =  new AreaDefinition(active_area_explicit_ghosts, *area_spec);
        CS_Area = new ClusterSequenceArea(*jet_constituents, *jet_def, *area_def);
        jets = JetCuts(no_ghost(CS_Area->inclusive_jets()));

        bkg_jet_def = new JetDefinition(kt_algorithm, R, BIpt2_scheme, Best);
        mBGE = new JetMedianBackgroundEstimator(bkg_selector, *bkg_jet_def, *area_def);
        mBGE->set_particles(*jet_constituents);
        _JetEvent->EventRho = mBGE->rho();
        _JetEvent->EventSigma = mBGE->sigma();
        //if(doBkgSubtraction){
        //    subtractor = new contrib::ConstituentSubtractor(mBGE);
        //    subtractor->set_common_bge_for_rho_and_rhom(true);
        //}
    }else{
        CS = new ClusterSequence(*jet_constituents, *jet_def);
        jets = JetCuts(CS->inclusive_jets());
    }

    contrib::ConstituentSubtractor subtractor(mBGE);

    if(jets.size() < 1)return kStOK;

    _JetEvent->NJets = jets.size();
   // _JetEvent->Jets = jets.size();
    _JetEvent->ClearVectors();

    for(int ijet= 0; ijet<jets.size(); ijet++){//BEGIN jet loop
        PseudoJet jet = jets[ijet];
        StJet _jet(jet.pt(), jet.eta(), jet.phi(), jet.e());
        if(doBackgroundCalc){
            _jet.Area = jet.area_4vector().pt();
            _jet.Rho = mBGE->rho(jet);
            _jet.Sigma = mBGE->sigma(jet);
            if(doBkgSubtraction){
                jet = subtractor(jets[ijet]);
                _jet.PtCorr = jet.pt();
        }
    }
        cout<<_jet.Pt<<" "<<_jet.Eta<<endl;
        vector<PseudoJet> constits = jet.constituents();
        _jet.NConstituents = constits.size();

        double neutralPt = 0;
        for(int ic = 0; ic < constits.size(); ic++){
            if(doBackgroundCalc && constits[ic].is_pure_ghost())continue;

            StJetConstituent _constit(ijet);
            _constit.EventArrayPos = constits[ic].user_info<UserInfo>().getIndex();
            _constit.Charge = constits[ic].user_info<UserInfo>().getCharge();   
            _constit.Pt = constits[ic].pt();
            _constit.Eta = constits[ic].eta();
            _constit.Phi = constits[ic].phi();
            _constit.E = constits[ic].e();

            _JetEvent->AddConstituent(_constit);

            if(_constit.Charge == 0){
                neutralPt += _constit.Pt;
            } 
        }
        _jet.NEF = neutralPt/_jet.Pt;
        _JetEvent->AddJet(_jet);
    }

    cout<<"# jets: "<<_JetEvent->NJets<<" Rho: "<<_JetEvent->EventRho<<endl;

    tree->Fill();

    return kStOK;
}

void StMyJetMaker::BookTree(){
    if(OutputFileName == ""){
        cout<<"Trees are not being written to any file!"<<endl;
        tree = new TTree("Event_Jet_Info", "Tree with jet Info");
        tree->Branch("Event_Jets", "StJetEvent", &(_JetEvent)); 
    }else{
        cout<<"Writing jet tree to: "<<OutputFileName<<endl;
        fout = new TFile(OutputFileName.c_str(), "UPDATE");
        fout->cd();
        //fout->mkdir(GetName());
        tree = new TTree("Event_Jet_Info", "Tree with jet Info");
        tree->SetDirectory(gDirectory);
        tree->Branch("Jets", "StJetEvent", &(_JetEvent)); 
        cout<<"Jets Tree directory set"<<endl;
    }
}
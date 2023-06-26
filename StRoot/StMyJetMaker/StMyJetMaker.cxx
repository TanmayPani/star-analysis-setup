#include "StMyJetMaker.h"
//FastJet stuff
#include "FJ_includes.h"
//ROOT includes
#include "TClonesArray.h"
#include "TTree.h"
#include "TFile.h"
//My StRoot includes
#include "StRoot/StMyAnalysisMaker/StMyAnalysisMaker.h"
#include "StRoot/TStarEventClass/TStarEvent.h"
#include "StRoot/TStarEventClass/TStarJetEvent.h"
#include "StRoot/TStarEventClass/TStarJet.h"
#include "StRoot/TStarEventClass/TStarJetConstituent.h"
#include "StRoot/TStarEventClass/TStarTrack.h"
#include "StRoot/TStarEventClass/TStarTower.h"

using namespace fastjet;

using namespace std;

ClassImp(StMyJetMaker);

StMyJetMaker::StMyJetMaker(string name, string analysis, string output): 
StMaker(name.c_str()){
    Analysis = analysis;
    OutputFileName = output;  
}

StMyJetMaker::~StMyJetMaker(){

}

//User Info class to include non-kinematic details of the particles into the pseudojet objects...
class StMyJetMaker::StJetUserInfo : public PseudoJet::UserInfoBase{
public:
	StJetUserInfo(const int indx, const short ch):_index(indx), _charge(ch){}
    short getIndex() const {return _index;}
	short getCharge() const {return _charge;}
protected:
    int _index;
	short _charge;
};

Int_t StMyJetMaker::Init(){

    jet_def = new JetDefinition(antikt_algorithm, R, BIpt2_scheme, Best);

    area_spec = new GhostedAreaSpec(MaxRap);
    area_def = new AreaDefinition(active_area_explicit_ghosts, *area_spec);

    bkg_jet_def = new JetDefinition(kt_algorithm, R, BIpt2_scheme, Best);
    Selector bkg_selector = SelectorAbsRapMax(3.0)*(!SelectorNHardest(2));
    mBGE = new JetMedianBackgroundEstimator(bkg_selector, *bkg_jet_def, *area_def);

    _JetEvent = new TStarJetEvent();
     
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
   
    myEvent = static_cast<TStarEvent*>(anaMaker->getEvent()); 

    //_JetEvent->SetEvent(*myEvent);

    //if(!myEvent->IsHT())return kStOK;

    vector<PseudoJet> full_event;
    vector<PseudoJet> jet_constituents;

    _Tracks = static_cast<TClonesArray*>(myEvent->getTracks());

    for(int itrk = 0; itrk < _Tracks->GetEntriesFast(); itrk++){
        TStarTrack *_track = static_cast<TStarTrack*>(_Tracks->At(itrk)); 

        full_event.push_back(PseudoJet(*_track));

        if(_track->pt() < JetConstituentMinPt)continue;  
        PseudoJet constit(*_track);
        constit.set_user_info(new StJetUserInfo(_track->index(), _track->charge()));
        jet_constituents.push_back(move(constit));
    }

    _Towers = static_cast<TClonesArray*>(myEvent->getTowers());

    for(int itow = 0; itow < _Towers->GetEntriesFast(); itow++){
        TStarTower *_tower = static_cast<TStarTower*>(_Towers->At(itow));

        full_event.push_back(PseudoJet(*_tower));

        if(_tower->et() < JetConstituentMinPt)continue;
        PseudoJet constit(*_tower);
        constit.set_user_info(new StJetUserInfo(_tower->index(), 0));
        jet_constituents.push_back(move(constit));
    } 
   
    Selector JetCuts = SelectorPtMin(JetPtMin)*SelectorAbsEtaMax(JetAbsEtaMax);

    vector<PseudoJet> _jets;

    if(!doMCJets){
        mBGE->set_particles(full_event); 
        CS_Area = new ClusterSequenceArea(jet_constituents, *jet_def, *area_def);
        Selector no_ghost = !SelectorIsPureGhost();
        _jets = JetCuts(no_ghost(CS_Area->inclusive_jets()));
    }else{
        CS = new ClusterSequence(jet_constituents, *jet_def);
        _jets = JetCuts(CS->inclusive_jets());
    }

    //jets = static_cast<vector<PseudoJet>*>(&_jets);
    
    if(_jets.size() < 1)return kStOK;

    _JetEvent->setNumberOfJets(_jets.size());

    if(!doMCJets){
        _JetEvent->setRho(mBGE->rho());
        _JetEvent->setSigma(mBGE->sigma());
    }
    _JetEvent->clearJetArray();

    if(_JetEvent->Jets->GetEntriesFast() > 0){
        cout<<"Jet array not cleared from previous event!"<<endl;
    }

    int ijet = 0;

    for(PseudoJet& _jet : _jets){//BEGIN jet loop
        TStarJet *jet = _JetEvent->addJet();
        jet->setIndex(ijet++);
        jet->setPtEtaPhiE(_jet.pt(), _jet.eta(), _jet.phi(), _jet.e());
        jet->setArea(_jet.area(), _jet.area_4vector().px(), _jet.area_4vector().py(), _jet.area_4vector().pz());
        //cout<<_jet.pt()<<" "<<jet->Pt()<<endl;
        if(!doMCJets){ 
            jet->setLocalRho(mBGE->rho(_jet));
            jet->setLocalSigma(mBGE->sigma(_jet));
        }
        for(PseudoJet& _con : _jet.constituents()){
            if(_con.is_pure_ghost())continue;
            TStarJetConstituent *con = jet->addConstituent();
            con->setIndex(_con.user_info<StJetUserInfo>().getIndex());
            con->setPtEtaPhiE(_con.pt(), _con.eta(), _con.phi(), _con.e());
            con->setCharge(_con.user_info<StJetUserInfo>().getCharge());
        }   
    }

    //cout<<"# jets: "<<_JetEvent->NumberOfJets()<<" Rho: "<<_JetEvent->Rho()<<endl;

    tree->Fill();

    if(CS){
        delete CS;
        CS = nullptr;
    }

    if(CS_Area){
        delete CS_Area;
        CS_Area = nullptr;
    }

    return kStOK;
}

void StMyJetMaker::BookTree(){
    if(OutputFileName == ""){
        cout<<"Trees are not being written to any file!"<<endl;
        tree = new TTree("Event_Jet_Info", "Tree with jet Info");
        tree->Branch("Event_Jets", &(_JetEvent)); 
    }else{
        cout<<"Writing jet tree to: "<<OutputFileName<<endl;
        fout = new TFile(OutputFileName.c_str(), "UPDATE");
        fout->cd();
        //fout->mkdir(GetName());
        tree = new TTree("Event_Jet_Info", "Tree with jet Info");
        tree->SetDirectory(gDirectory);
        tree->Branch("Event_Jets", &(_JetEvent)); 
        cout<<"Jets Tree directory set"<<endl;
    }
}
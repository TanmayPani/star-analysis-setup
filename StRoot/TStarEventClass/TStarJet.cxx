#define TStarJet_cxx

#include "TStarJet.h"
#include "TStarJetConstituent.h"
#include "FJ_includes.h"
#include "TClonesArray.h"

ClassImp(TStarJet);

using namespace std;
using namespace fastjet;

TStarJet::TStarJet(){
    Constituents = new TClonesArray("TStarJetConstituent", 50);
}

TStarJet::TStarJet(PseudoJet& _jet){
    _E = _jet.E();
    _Px = _jet.px();
    _Py = _jet.py();
    _Pz = _jet.pz();

    Constituents = new TClonesArray("TStarJetConstituent", 50);
}

TStarJet::TStarJet(double e, double px, double py, double pz){
    _E = e;
    _Px = px;
    _Py = py;
    _Pz = pz;
    Constituents = new TClonesArray("TStarJetConstituent", 50);
}

TStarJet::~TStarJet(){
    if(!TestBit(kIsOnHeap)){
        delete Constituents;
    }
}

double TStarJet::Phi(){
    double phi = Phi_Std();
    if(phi < 0.0) return phi+2.0*TMath::Pi();
    else if(phi > 2.0*TMath::Pi()) return phi-2.0*TMath::Pi();
    else return phi; 
}

void TStarJet::SetJet(PseudoJet& jet){
    _E = jet.e();
    _Px = jet.px();
    _Py = jet.py();
    _Pz = jet.pz();
    
    SetArea(jet);
}

void TStarJet::SetArea(PseudoJet& jet){
    if(jet.has_area()){
        _A = jet.area();
        _Ax = jet.area_4vector().px();
        _Ay = jet.area_4vector().py();
        _Az = jet.area_4vector().pz();
    }
}

TStarJetConstituent* TStarJet::AddConstituent(PseudoJet& _con){
    if(_con.is_pure_ghost())return nullptr;
    int index = Constituents->GetEntriesFast();
    TStarJetConstituent* _constit 
            = static_cast<TStarJetConstituent*>(Constituents->ConstructedAt(index));
    _constit->SetConstituent(_con);
    return _constit;
}

void TStarJet::ClearConstituentArray(){
    Constituents->Clear();
}
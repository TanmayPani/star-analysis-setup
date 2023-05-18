#define TStarJetConstituent_cxx

#include "TStarJetConstituent.h"
#include "FJ_includes.h"

using namespace fastjet;
using namespace std;

ClassImp(TStarJetConstituent);

TStarJetConstituent::TStarJetConstituent(){

}

TStarJetConstituent::TStarJetConstituent(float e, float px, float py, float pz){
    _E = e;
    _Px = px;
    _Py = py;
    _Pz = pz;
}

TStarJetConstituent::TStarJetConstituent(PseudoJet& _con){
    _E  = _con.E();
    _Px = _con.px();
    _Py = _con.py();
    _Pz = _con.pz();
}

TStarJetConstituent::~TStarJetConstituent(){

}

float TStarJetConstituent::Phi(){
    float phi = Phi_Std();
    if(phi < 0.0) return phi+2.0*TMath::Pi();
    else if(phi > 2.0*TMath::Pi()) return phi-2.0*TMath::Pi();
    else return phi; 
}

void TStarJetConstituent::SetConstituent(PseudoJet& _con){
    _E  = _con.E();
    _Px = _con.px();
    _Py = _con.py();
    _Pz = _con.pz();
}


#define TStarJet_cxx

#include "TStarJet.h"

ClassImp(TStarJet);

using namespace std;

TStarJet::TStarJet(){
}

TStarJet::TStarJet(unsigned int i, float pt, float eta, float phi, float e) : 
TStarVector(pt, eta, phi, e, i, -99){
}

TStarJet::TStarJet(const TStarJet& j) : TStarVector(j) {
    _A = j._A;
    _Ax = j._Ax;
    _Ay = j._Ay;
    _Az = j._Az;
    _Rho = j._Rho;
    _Sigma = j._Sigma;
}

TStarJet::~TStarJet(){

}

void TStarJet::setJet(const TStarJet& jet){
    set(jet);
    _A = jet._A;
    _Ax = jet._Ax;
    _Ay = jet._Ay;
    _Az = jet._Az;
    _Rho = jet._Rho;
    _Sigma = jet._Sigma;
}
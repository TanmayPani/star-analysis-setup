#define TStarJetConstituent_cxx

#include "TStarJetConstituent.h"

using namespace std;

ClassImp(TStarJetConstituent);

TStarJetConstituent::TStarJetConstituent(unsigned int ijet){
    _Index = ijet;
}

TStarJetConstituent::TStarJetConstituent(unsigned int ijet, signed char ch, float _pt, float _eta, float _phi, float _e) :
TStarVector(_pt, _eta, _phi, _e){
    _Index = ijet;
    _Charge = ch;
}

TStarJetConstituent::TStarJetConstituent(unsigned int ijet, signed char ch, TStarVector& v) : TStarVector(v){
    _Index = ijet;
    _Charge = ch;
}

TStarJetConstituent::TStarJetConstituent(const TStarJetConstituent& c) : TStarVector(c){
    _Index = c._Index;
    _Charge = c._Charge;
}

TStarJetConstituent::~TStarJetConstituent(){

}
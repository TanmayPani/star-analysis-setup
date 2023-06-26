#define TStarTower_cxx

#include "TStarTower.h"
#include "TVector3.h"

#include <iostream>

ClassImp(TStarTower);

using namespace std;

TStarTower::TStarTower(){

}

TStarTower::TStarTower(unsigned int Id, unsigned int adc, double E){
    _Index = Id;
    _ADC = adc;
    _RawE = E;
}

TStarTower::TStarTower(unsigned int Id, unsigned int adc, double eraw, double E, TVector3& towPos, double m){
    _Index = Id;
    _ADC = adc;
    _RawE = eraw;
    setEtaPhiEM(towPos, E, m);
}

TStarTower::TStarTower(const TStarTower& t) : TStarVector(t){
    _Index = t._Index;
    _ADC = t._ADC;
    _RawE = t._RawE;
    _NMatchedTracks = t._NMatchedTracks;
}

TStarTower::TStarTower(unsigned int Id, unsigned int adc, double eraw, TStarVector& v) : TStarVector(v){
    _Index = Id;
    _ADC = adc;
    _RawE = eraw;
}

TStarTower::~TStarTower(){

}

void TStarTower::print(){
    cout<< "TStarTower :"<<endl;
    cout << "Tower " << _Index << " ADC: " << _ADC << " RawE: " << _RawE << " E: " << _E << endl;
    TStarVector::print();
}

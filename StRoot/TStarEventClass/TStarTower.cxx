#define TStarTower_cxx

#include <iostream>
#include "TStarTower.h"
#include "TVector3.h"

ClassImp(TStarTower);

using namespace std;

TStarTower::TStarTower(){

}

TStarTower::TStarTower(unsigned int Id, unsigned int adc, float e){
    _Index = Id;
    _ADC = adc;
    _E = e;
}

TStarTower::TStarTower(unsigned int Id, unsigned int adc, float e, TVector3& towPos){
    _Index = Id;
    _ADC = adc;
    _E = e;
    _X = towPos.x();
    _Y = towPos.y();
    _Z = towPos.z();
}

TStarTower::~TStarTower(){

}

float TStarTower::Px(){
    float r = sqrt(_X*_X + _Y*_Y + _Z*_Z);   
    return P()*_X/r;
}

float TStarTower::Py(){
    float r = sqrt(_X*_X + _Y*_Y + _Z*_Z);
    return P()*_Y/r;
}

float TStarTower::Pz(){
    float r = sqrt(_X*_X + _Y*_Y + _Z*_Z);
    return P()*_Z/r;
}

float TStarTower::Phi(){
   float phi = atan2(_Y, _X);
   if(phi < 0.0) return phi+2.0*TMath::Pi();
   else if(phi > 2.0*TMath::Pi()) return phi-2.0*TMath::Pi();
   else return phi; 
}

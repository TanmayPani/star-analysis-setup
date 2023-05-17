#define TStarTower_cxx

#include <iostream>
#include "TStarTower.h"
#include "TVector3.h"

ClassImp(TStarTower);

using namespace std;

TStarTower::TStarTower(){

}

TStarTower::TStarTower(int& Id, int& adc, double& e){
    _Index = Id;
    _ADC = adc;
    _E = e;
}

TStarTower::TStarTower(int& Id, int& adc, double& e, TVector3& towPos){
    _Index = Id;
    _ADC = adc;
    _E = e;
    _X = towPos.x();
    _Y = towPos.y();
    _Z = towPos.z();
}

TStarTower::~TStarTower(){

}

double TStarTower::Px(){
    double r = sqrt(_X*_X + _Y*_Y + _Z*_Z);   
    return P()*_X/r;
}

double TStarTower::Py(){
    double r = sqrt(_X*_X + _Y*_Y + _Z*_Z);
    return P()*_Y/r;
}

double TStarTower::Pz(){
    double r = sqrt(_X*_X + _Y*_Y + _Z*_Z);
    return P()*_Z/r;
}

double TStarTower::Phi(){
   double phi = atan2(_Y, _X);
   if(phi < 0.0) return phi+2.0*TMath::Pi();
   else if(phi > 2.0*TMath::Pi()) return phi-2.0*TMath::Pi();
   else return phi; 
}

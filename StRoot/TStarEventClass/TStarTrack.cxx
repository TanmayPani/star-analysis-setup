#define TStarTrack_cxx

#include "TStarTrack.h"
#include "TVector3.h"

#include <iostream>

ClassImp(TStarTrack);

using namespace std;

TStarTrack::TStarTrack(){

}

TStarTrack::TStarTrack(unsigned int _i, short _ch, double _pt, double _eta, double _phi, double _m) : 
TStarVector(_pt, _eta, _phi, sqrt(p2() + _m * _m)){
    _Index = _i;
    _Charge = _ch;
}

TStarTrack::TStarTrack(unsigned int i, short ch, TVector3& trkMom, double m) : 
TStarVector(trkMom, sqrt(trkMom.Mag2() + m * m)){
    _Index = i;
    _Charge = ch;
}

TStarTrack::TStarTrack(const TStarTrack& t) : TStarVector(t){
    _Index = t._Index;
    _Charge = t._Charge;
    _trackingEff = t._trackingEff;
    _MatchedTowerIndex = t._MatchedTowerIndex;
    _nSigmaPion = t._nSigmaPion;
    _nSigmaKaon = t._nSigmaKaon;
    _nSigmaProton = t._nSigmaProton;
    _nSigmaElectron = t._nSigmaElectron;
}

TStarTrack::~TStarTrack(){
    
}

void TStarTrack::print(){
    cout << "TStarTrack: " << endl;
    cout <<"Index: " << _Index <<" Charge: " << _Charge << endl;
    cout << "Tracking Efficiency: " << _trackingEff<< " Matched Tower Index: " << _MatchedTowerIndex << endl;
    cout << "nSigmaPion: " << _nSigmaPion << " nSigmaKaon: " << _nSigmaKaon <<" nSigmaProton: " << _nSigmaProton <<" nSigmaElectron: " << _nSigmaElectron << endl;
    TStarVector::print();
}


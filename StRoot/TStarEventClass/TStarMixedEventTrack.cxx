#define TStarMixedEventTrack_cxx

#include "TStarMixedEventTrack.h"

ClassImp(TStarMixedEventTrack);

TStarMixedEventTrack::TStarMixedEventTrack(){
    _Pt = -1;
    _Eta = -999;
    _Phi = -999;
    _Charge = -99;
}

TStarMixedEventTrack::TStarMixedEventTrack(double _pt, double _eta, double _phi, short ch){
    _Pt = _pt;
    _Eta = _eta;
    _Phi = _phi;
    _Charge = ch;
}

void TStarMixedEventTrack::_force_phi_02pi(){
    if(_Phi < 0) _Phi += 2 * TMath::Pi();
    if(_Phi > 2 * TMath::Pi()) _Phi -= 2 * TMath::Pi();
}
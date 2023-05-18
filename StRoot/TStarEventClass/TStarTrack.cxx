#define TStarTrack_cxx

#include "TStarTrack.h"

#include "TVector3.h"

ClassImp(TStarTrack);

using namespace std;

TStarTrack::TStarTrack(){

}

TStarTrack::TStarTrack(unsigned int i, signed char ch, float px, float py, float pz){
    _Index = i;
    _Charge = ch;
    _Px = px;
    _Py = py;
    _Pz = pz;
}

TStarTrack::TStarTrack(unsigned int i, signed char ch, TVector3& trkMom){
    _Index = i;
    _Charge = ch;
    _Px = trkMom.Px();
    _Py = trkMom.Py();
    _Pz = trkMom.Pz();
}

TStarTrack::~TStarTrack(){

}

float TStarTrack::Phi(){
   float phi = atan2(_Py, _Px);
   if(phi < 0.0) return phi+2.0*TMath::Pi();
   else if(phi > 2.0*TMath::Pi()) return phi-2.0*TMath::Pi();
   else return phi; 
}

void TStarTrack::DoTrackPid(float nsig_pi, float nsig_K, float nsig_p, float nsig_e){
    if(abs(nsig_pi) < 2)_IsPion = true; 
    if(abs(nsig_K) < 2) _IsKaon = true; 
    if(abs(nsig_p) < 2) _IsProton = true; 
    if(abs(nsig_e) < 2) _IsElectron = true; 
}




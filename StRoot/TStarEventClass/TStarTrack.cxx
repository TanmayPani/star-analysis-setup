#define TStarTrack_cxx

#include "TStarTrack.h"

#include "TVector3.h"

ClassImp(TStarTrack);

using namespace std;

TStarTrack::TStarTrack(){

}

TStarTrack::TStarTrack(int& i, short& ch, double& px, double& py, double& pz){
    _Index = i;
    _Charge = ch;
    _Px = px;
    _Py = py;
    _Pz = pz;
}

TStarTrack::TStarTrack(int& i, short& ch, TVector3& trkMom){
    _Index = i;
    _Charge = ch;
    _Px = trkMom.Px();
    _Py = trkMom.Py();
    _Pz = trkMom.Pz();
}

TStarTrack::~TStarTrack(){

}

double TStarTrack::Phi(){
   double phi = atan2(_Py, _Px);
   if(phi < 0.0) return phi+2.0*TMath::Pi();
   else if(phi > 2.0*TMath::Pi()) return phi-2.0*TMath::Pi();
   else return phi; 
}

void TStarTrack::DoTrackPid(double nsig_pi, double nsig_K, double nsig_p, double nsig_e){
    if(abs(nsig_pi) < 2)_IsPion = true; 
    if(abs(nsig_K) < 2) _IsKaon = true; 
    if(abs(nsig_p) < 2) _IsProton = true; 
    if(abs(nsig_e) < 2) _IsElectron = true; 
}




#ifndef TStarTrack_h
#define TStarTrack_h

#include "TObject.h"
#include "TMath.h"
#include "TVector3.h"

class TStarTrack : public TObject{
public:
    TStarTrack();
    TStarTrack(unsigned int i, signed char ch, float px, float py, float pz);
    TStarTrack(unsigned int i, signed char ch, TVector3& t);

    virtual ~TStarTrack();

    float Px() {return _Px;}
    float Py() {return _Py;}
    float Pz() {return _Pz;}
    signed char Charge() {return _Charge;}
    unsigned int Index() {return _Index;}
    unsigned int MatchedTower() {return _MatchedTowerIndex;}
    float TrackingEfficiency() {return _trackingEff;}

    float P()  {return std::sqrt(_Px*_Px + _Py*_Py + _Pz*_Pz);}
    float Pt() {return std::sqrt(_Px*_Px + _Py*_Py);}
    float Theta() {return std::atan2(std::sqrt(_Px*_Px + _Py*_Py), abs(_Pz));}
    float Eta() {return -1.0*std::log(std::tan(0.5*Theta()));}
    float Phi();// \phi \in [0, 2\pi]
    float Phi_Std(){return std::atan2(_Py, _Px);} // \phi \in [-\pi, \pi]

    float pi0mass = 0.13957;
    float Pi0E() {return std::sqrt(_Px*_Px + _Py*_Py + _Pz*_Pz + pi0mass*pi0mass);}
    float GetE(float mass) {return std::sqrt(_Px*_Px + _Py*_Py + _Pz*_Pz + mass*mass);} 

    void DoTrackPid(float _pi, float _k, float _p, float _e);

    void SetPx(float p) {_Px = p;}
    void SetPy(float p) {_Py = p;}
    void SetPz(float p) {_Pz = p;}
    void SetPxPyPz(float px, float py, float pz){_Px = px; _Py = py; _Pz = pz;}
    void SetPxPyPz(TVector3& p){_Px = p.Px(); _Py = p.Py(); _Pz = p.Pz();}
    void SetIndex(unsigned int i) {_Index = i;}
    void SetCharge(signed char ch) {_Charge = ch;}
    void SetMatchedTower(unsigned int i) {_MatchedTowerIndex = i;}
    void SetTrackingEfficiency(float eff) {_trackingEff = eff;}

    unsigned int _Index = 0;
    signed char _Charge = -99;
    float _Px = 0;
    float _Py = 0;
    float _Pz = 0;

    float _trackingEff = 0;
    unsigned int _MatchedTowerIndex = 0;

    bool _IsPion = false;
    bool _IsKaon = false;
    bool _IsProton = false;
    bool _IsElectron = false;

    ClassDef(TStarTrack, 1)
};


#endif
#ifndef TStarTrack_h
#define TStarTrack_h

#include "TObject.h"
#include "TMath.h"
#include "TVector3.h"

class TStarTrack : public TObject{
public:
    TStarTrack();
    TStarTrack(int& i, short& ch, double& px, double& py, double& pz);
    TStarTrack(int& i, short& ch, TVector3& t);

    virtual ~TStarTrack();

    double Px() {return _Px;}
    double Py() {return _Py;}
    double Pz() {return _Pz;}
    short Charge() {return _Charge;}
    int Index() {return _Index;}
    int MatchedTower() {return _MatchedTowerIndex;}
    double TrackingEfficiency() {return _trackingEff;}

    double P()  {return std::sqrt(_Px*_Px + _Py*_Py + _Pz*_Pz);}
    double Pt() {return std::sqrt(_Px*_Px + _Py*_Py);}
    double Theta() {return std::atan2(std::sqrt(_Px*_Px + _Py*_Py), abs(_Pz));}
    double Eta() {return -1.0*std::log(std::tan(0.5*Theta()));}
    double Phi();// \phi \in [0, 2\pi]
    double Phi_Std(){return std::atan2(_Py, _Px);} // \phi \in [-\pi, \pi]

    double pi0mass = 0.13957;
    double Pi0E() {return std::sqrt(_Px*_Px + _Py*_Py + _Pz*_Pz + pi0mass*pi0mass);}
    double GetE(double mass) {return std::sqrt(_Px*_Px + _Py*_Py + _Pz*_Pz + mass*mass);} 

    void DoTrackPid(double pi, double k, double p, double e);

    void SetPx(double p) {_Px = p;}
    void SetPy(double p) {_Py = p;}
    void SetPz(double p) {_Pz = p;}
    void SetPxPyPz(double px, double py, double pz){_Px = px; _Py = py; _Pz = pz;}
    void SetPxPyPz(TVector3& p){_Px = p.Px(); _Py = p.Py(); _Pz = p.Pz();}
    void SetIndex(int& i) {_Index = i;}
    void SetCharge(short ch) {_Charge = ch;}
    void SetMatchedTower(int& i) {_MatchedTowerIndex = i;}
    void SetTrackingEfficiency(double& eff) {_trackingEff = eff;}

    int _Index = 0;
    short _Charge = -99;
    double _Px = 0;
    double _Py = 0;
    double _Pz = 0;

    double _trackingEff = 0;
    int _MatchedTowerIndex = -1;

    bool _IsPion = false;
    bool _IsKaon = false;
    bool _IsProton = false;
    bool _IsElectron = false;

    ClassDef(TStarTrack, 1)
};


#endif
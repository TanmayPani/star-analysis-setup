#ifndef TStarTrack_h
#define TStarTrack_h

#include "TStarVector.h"

class TVector3;

class TStarTrack : public TStarVector{
public:
    TStarTrack();
    TStarTrack(unsigned int _i, short _ch, double _pt, double _eta, double _phi, double _m);
    TStarTrack(unsigned int i, short ch, TVector3& t, double m);
    TStarTrack(const TStarTrack& t);
    virtual ~TStarTrack();

    signed char charge() {return _Charge;}
    unsigned int index() {return _Index;}
    unsigned int matchedTower() {return _MatchedTowerIndex;}
    double trackingEfficiency() {return _trackingEff;}

    void setIndex(unsigned int i) {_Index = i;}
    void setCharge(signed char ch) {_Charge = ch;}
    void setMatchedTower(unsigned int i) {_MatchedTowerIndex = i;}
    void setTrackingEfficiency(double eff) {_trackingEff = eff;}
    void setTrackVector(double _pt, double _eta, double _phi, double _m){setPtEtaPhiM(_pt, _eta, _phi, _m);}
    void setNSigmas(double nSPion, double nSKaon, double nSProton, double nSElectron){_nSigmaPion = nSPion; _nSigmaKaon = nSKaon; _nSigmaProton = nSProton; _nSigmaElectron = nSElectron;}

    virtual void print();

    unsigned int _Index = 0;
    short _Charge = -99;
    double _trackingEff = 0;
    unsigned int _MatchedTowerIndex = 0;

    double _nSigmaPion = 0; //!
    double _nSigmaKaon = 0; //!
    double _nSigmaProton = 0; //!
    double _nSigmaElectron = 0; //!

    ClassDef(TStarTrack, 1)
};


#endif
#ifndef TStarTrack_h
#define TStarTrack_h

#include "TStarVector.h"

class TVector3;

class TStarTrack : public TStarVector{
public:
    TStarTrack();
    TStarTrack(const TStarTrack& t);
    virtual ~TStarTrack();

    int genMatchId() {return _GenMatchId;}
    unsigned int matchedTower() {return _MatchedTowerIndex;}
    double trackingEfficiency() {return _trackingEff;}

    virtual void setTrack(const TStarTrack& t);
    
    void setGenMatchId(int id) {_GenMatchId = id;}
    void setMatchedTower(unsigned int i) {_MatchedTowerIndex = i;}
    void setTrackingEfficiency(double eff) {_trackingEff = eff;}
    void setNSigmas(double nSPion, double nSKaon, double nSProton, double nSElectron){_nSigmaPion = nSPion; _nSigmaKaon = nSKaon; _nSigmaProton = nSProton; _nSigmaElectron = nSElectron;}

    virtual void Print(Option_t* option = "") const;

    int _GenMatchId = -1;//!
    double _trackingEff = 0; 
    unsigned int _MatchedTowerIndex = 0;

    double _nSigmaPion = 0; //!
    double _nSigmaKaon = 0; //!
    double _nSigmaProton = 0; //!
    double _nSigmaElectron = 0; //!

    ClassDef(TStarTrack, 1)
};


#endif
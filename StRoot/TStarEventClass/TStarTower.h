#ifndef TStarTower_h
#define TStarTower_h

#include "TStarVector.h"

class TVector3;

class TStarTower : public TStarVector{
public:
    TStarTower();
    TStarTower(unsigned int id, unsigned int adc, double RawE);
    TStarTower(unsigned int id, unsigned int adc, double RawE, double E, TVector3& pos, double m);
    TStarTower(const TStarTower& t);
    TStarTower(unsigned int id, unsigned int adc, double RawE, TStarVector& v);
    virtual ~TStarTower();

    unsigned int index() {return _Index;}
    unsigned int ADC() {return _ADC;}
    double uncorrectedE() {return _RawE;}
    unsigned int nMatchedTracks() {return _NMatchedTracks;}

    void setIndex(int i) {_Index = i;}
    void setADC(int adc) {_ADC = adc;}
    void setRawE(double e) {_RawE = e;}
    void setNMatchedTracks(int n) {_NMatchedTracks = n;}
    void setTowerVector(double _eta, double _phi, double _e, double _m){setEtaPhiEM(_eta, _phi, _e, _m);}

    virtual void print();

    unsigned int _Index = 0;
    unsigned int _ADC = 0;
    double _RawE = 0;
    unsigned int _NMatchedTracks = 0;

    ClassDef(TStarTower, 1)
};

#endif
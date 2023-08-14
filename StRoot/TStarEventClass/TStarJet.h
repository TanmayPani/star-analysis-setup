#ifndef TStarJet_h
#define TStarJet_h

#include <map>

#include "TStarVector.h"

class TStarJet : public TStarVector {
public:
    TStarJet();
    TStarJet(unsigned int i, float pt, float eta, float phi, float e);
    TStarJet(const TStarJet& j);
    virtual ~TStarJet();

    float area(){return _A;}
    float Ax(){return _Ax;}
    float Ay(){return _Ay;}
    float Az(){return _Az;}
    float localRho(){return _Rho;}
    float localSigma(){return _Sigma;}

    virtual void setJet(const TStarJet& jet);

    void setArea(float a, float ax = 0, float ay = 0, float az = 0){_A = a; _Ax = ax; _Ay = ay; _Az = az;}   
    void setLocalRho(float rho){_Rho = rho;}
    void setLocalSigma(float sig){_Sigma = sig;}

    float _A = 0;//!
    float _Ax = 0;//!
    float _Ay = 0;//!
    float _Az = 0;//!
    float _Rho = 0;//!
    float _Sigma = 0;//!

    ClassDef(TStarJet, 1);
};

#endif
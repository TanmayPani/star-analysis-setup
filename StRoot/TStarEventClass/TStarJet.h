#ifndef TStarJet_h
#define TStarJet_h

//#include <stdlib.h>
//#include <cmath>

#include "TStarVector.h"

class TClonesArray;
class TStarJetConstituent;

class TStarJet : public TStarVector {
public:
    TStarJet();
    TStarJet(unsigned int i, float px, float py, float pz, float e);
    TStarJet(const TStarJet& j);
    virtual ~TStarJet();

    unsigned int index(){return _Index;}
    float area(){return _A;}
    float Ax(){return _Ax;}
    float Ay(){return _Ay;}
    float Az(){return _Az;}
    float localRho(){return _Rho;}
    float localSigma(){return _Sigma;}
    TClonesArray* getConstituents(){return Constituents;}

    void setIndex(unsigned int i) {_Index = i;}
    void setArea(float a, float ax = 0, float ay = 0, float az = 0){_A = a; _Ax = ax; _Ay = ay; _Az = az;}   
    TStarJetConstituent* addConstituent();
    void addConstituent(const TStarJetConstituent& c);

    void setLocalRho(float rho){_Rho = rho;}
    void setLocalSigma(float sig){_Sigma = sig;}

    void clearConstituentArray();

    unsigned int _Index = 0;
    float _PtSub = 0;
    float _A = 0;
    float _Ax = 0;
    float _Ay = 0;
    float _Az = 0;
    float _Rho = 0;
    float _Sigma = 0;
   
    TClonesArray *Constituents = nullptr;

    ClassDef(TStarJet, 1);
};

#endif
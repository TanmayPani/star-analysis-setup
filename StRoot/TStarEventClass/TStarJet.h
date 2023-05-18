#ifndef TStarJet_h
#define TStarJet_h

//#include <stdlib.h>
//#include <cmath>

#include "TObject.h"
#include "TMath.h"

class TClonesArray;
class TStarJetConstituent;

namespace fastjet{
    class PseudoJet;
}

class TStarJet : public TObject {
public:
    TStarJet();
    TStarJet(fastjet::PseudoJet& j);
    TStarJet(float px, float py, float pz, float e);
    virtual ~TStarJet();

    float E() {return _E;}
    float Px(){return _Px;}
    float Py(){return _Py;}
    float Pz(){return _Pz;}
    float P(){return std::sqrt(_Px*_Px + _Py*_Py + _Pz*_Pz);}
    float Pt(){return std::sqrt(_Px*_Px + _Py*_Py);}
    float Theta(){return std::atan2(std::sqrt(_Px*_Px + _Py*_Py), abs(_Pz));}
    float Eta(){return -1.0*std::log(std::tan(0.5*Theta()));}
    float Rap(){return 0.5*std::log((_E+_Pz)/(_E-_Pz));}
    float Phi();
    float Phi_Std(){return std::atan2(_Py, _Px);}
    float M2(){return _E*_E - _Px*_Px - _Py*_Py - _Pz*_Pz;}
    float Mt2(){return _E*_E - _Pz*_Pz;}
    float Et(){return _E*std::sin(Theta());}
    float Area(){return _A;}
    float Ax(){return _Ax;}
    float Ay(){return _Ay;}
    float Az(){return _Az;}
    float Rho(){return _Rho;}
    float Sigma(){return _Sigma;}
    TClonesArray* GetConstituents(){return Constituents;}

    void SetJet(fastjet::PseudoJet& j);
    void SetArea(fastjet::PseudoJet& j);   
    TStarJetConstituent* AddConstituent(fastjet::PseudoJet& c);

    void SetEPxPyPz(float e, float px, float py, float pz){
        _E = e; 
        _Px = px; 
        _Py = py; 
        _Pz = pz;
    }
    void SetAxAyAz(float ax, float ay, float az){
        _A = std::sqrt(ax*ax + ay*ay);
        _Ax = ax;
        _Ay = ay;
        _Az = az;
    }
    void SetRho(float rho){_Rho = rho;}
    void SetSigma(float sig){_Sigma = sig;}

    void ClearConstituentArray();

    float _E = 0;
    float _Px = 0;
    float _Py = 0;
    float _Pz = 0;
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
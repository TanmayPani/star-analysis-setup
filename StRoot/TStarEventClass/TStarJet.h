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
    TStarJet(double px, double py, double pz, double e);
    virtual ~TStarJet();

    double E() {return _E;}
    double Px(){return _Px;}
    double Py(){return _Py;}
    double Pz(){return _Pz;}
    double P(){return std::sqrt(_Px*_Px + _Py*_Py + _Pz*_Pz);}
    double Pt(){return std::sqrt(_Px*_Px + _Py*_Py);}
    double Theta(){return std::atan2(std::sqrt(_Px*_Px + _Py*_Py), abs(_Pz));}
    double Eta(){return -1.0*std::log(std::tan(0.5*Theta()));}
    double Rap(){return 0.5*std::log((_E+_Pz)/(_E-_Pz));}
    double Phi();
    double Phi_Std(){return std::atan2(_Py, _Px);}
    double M2(){return _E*_E - _Px*_Px - _Py*_Py - _Pz*_Pz;}
    double Mt2(){return _E*_E - _Pz*_Pz;}
    double Et(){return _E*std::sin(Theta());}
    double Area(){return _A;}
    double Ax(){return _Ax;}
    double Ay(){return _Ay;}
    double Az(){return _Az;}
    double Rho(){return _Rho;}
    double Sigma(){return _Sigma;}
    TClonesArray* GetConstituents(){return Constituents;}

    void SetJet(fastjet::PseudoJet& j);
    void SetArea(fastjet::PseudoJet& j);   
    TStarJetConstituent* AddConstituent(fastjet::PseudoJet& c);

    void SetEPxPyPz(double e, double px, double py, double pz){
        _E = e; 
        _Px = px; 
        _Py = py; 
        _Pz = pz;
    }
    void SetAxAyAz(double ax, double ay, double az){
        _A = std::sqrt(ax*ax + ay*ay);
        _Ax = ax;
        _Ay = ay;
        _Az = az;
    }
    void SetRho(double rho){_Rho = rho;}
    void SetSigma(double sig){_Sigma = sig;}

    void ClearConstituentArray();

    double _E = 0;
    double _Px = 0;
    double _Py = 0;
    double _Pz = 0;
    double _A = 0;
    double _Ax = 0;
    double _Ay = 0;
    double _Az = 0;
    double _Rho = 0;
    double _Sigma = 0;
    TClonesArray *Constituents = nullptr;

    ClassDef(TStarJet, 1);
};

#endif
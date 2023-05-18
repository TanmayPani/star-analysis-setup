#ifndef TStarJetConstituent_h
#define TStarJetConstituent_h

#include "TObject.h"
#include "TMath.h"

namespace fastjet{
    class PseudoJet;
}

class TStarJetConstituent : public TObject{
public:
    TStarJetConstituent();
    TStarJetConstituent(float e, float px, float py, float pz);
    TStarJetConstituent(fastjet::PseudoJet& c);
    virtual ~TStarJetConstituent();

    float E() {return _E;}
    float Px(){return _Px;}
    float Py(){return _Py;}
    float Pz(){return _Pz;}
    short Charge(){return _Charge;}
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

    void SetConstituent(fastjet::PseudoJet& c);
    void SetEPxPyPz(float e, float px, float py, float pz){
        _E = e; 
        _Px = px; 
        _Py = py; 
        _Pz = pz;
    }
    void SetCharge(short ch){_Charge = ch;}

    float _E = 0;
    float _Px = 0;
    float _Py = 0;
    float _Pz = 0;
    short _Charge = 0;

    ClassDef(TStarJetConstituent, 1);
};

#endif
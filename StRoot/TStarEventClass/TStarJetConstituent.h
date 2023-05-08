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
    TStarJetConstituent(double e, double px, double py, double pz);
    TStarJetConstituent(fastjet::PseudoJet& c);
    virtual ~TStarJetConstituent();

    double E() {return _E;}
    double Px(){return _Px;}
    double Py(){return _Py;}
    double Pz(){return _Pz;}
    short Charge(){return _Charge;}
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

    void SetConstituent(fastjet::PseudoJet& c);
    void SetEPxPyPz(double e, double px, double py, double pz){
        _E = e; 
        _Px = px; 
        _Py = py; 
        _Pz = pz;
    }
    void SetCharge(short ch){_Charge = ch;}

    double _E = 0;
    double _Px = 0;
    double _Py = 0;
    double _Pz = 0;
    short _Charge = 0;

    ClassDef(TStarJetConstituent, 1);
};

#endif
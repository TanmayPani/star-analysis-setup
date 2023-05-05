#ifndef TStarTower_h
#define TStarTower_h

#include <vector>

#include "TMath.h"
#include "TObject.h"
#include "TVector3.h"

class TStarTower : public TObject{
public:
    TStarTower();
    TStarTower(int& id, int& adc, double& E);
    TStarTower(int& id, int& adc, double& E, TVector3& pos);
    virtual ~TStarTower();

    double X() {return _X;}
    double Y() {return _Y;}
    double Z() {return _Z;}
    double E() {return _E;}
    double RawE() {return _RawE;}
    bool IsHT0() {return _IsHT0;}
    bool IsHT1() {return _IsHT1;}
    bool IsHT2() {return _IsHT2;}
    bool IsHT3() {return _IsHT3;}
    int Index() {return _Index;}
    int ADC() {return _ADC;}
    int NMatchedTracks() {return _NMatchedTracks;}
    
    double mass = 0.13957;
    void SetMass(double m){mass = m;}

    double P(){return std::sqrt(_E*_E - mass*mass);} 
    double Px(); 
    double Py(); 
    double Pz(); 
    double Pt() {return std::sqrt(Px()*Px() + Py()*Py());} 
    double Theta() {return std::atan2(std::sqrt(_X*_X + _Y*_Y), std::abs(_Z));}
    double Eta() {return -1.0*std::log(std::tan(0.5*Theta()));}
    double Phi();// \phi \in [0, 2\pi]
    double Phi_Std(){return std::atan2(_Y, _X);} // \phi \in [-\pi, \pi] 

    void SetHighTowerStatus(std::vector<bool> htstat);
    void SetIndex(int i) {_Index = i;}
    void SetADC(int adc) {_ADC = adc;}
    void SetE(double e) {_E = e;}
    void SetRawE(double e) {_RawE = e;}
    void SetNMatchedTracks(int n) {_NMatchedTracks = n;}
    void SetPosX(double x) {_X = x;}
    void SetPosY(double y) {_Y = y;}
    void SetPosZ(double z) {_Z = z;}
    inline void SetPosXYZ(double x, double y, double z) {_X = x; _Y = y; _Z = z;}
    inline void SetPosXYZ(TVector3& pos){_X = pos.x(); _Y = pos.y(); _Z = pos.z();}

    int _Index = 0;
    int _ADC = 0;
    double _E = 0;
    double _RawE = 0;
    double _X = 0;
    double _Y = 0;
    double _Z = 0;
    bool _IsHT0 = false;
    bool _IsHT1 = false;
    bool _IsHT2 = false;
    bool _IsHT3 = false;
    int _NMatchedTracks = 0;

    ClassDef(TStarTower, 1)
};

#endif
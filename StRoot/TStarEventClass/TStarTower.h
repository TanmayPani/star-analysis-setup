#ifndef TStarTower_h
#define TStarTower_h

#include <vector>

#include "TMath.h"
#include "TObject.h"
#include "TVector3.h"

class TStarTower : public TObject{
public:
    TStarTower();
    TStarTower(unsigned int id, unsigned int adc, float E);
    TStarTower(unsigned int id, unsigned int adc, float E, TVector3& pos);
    virtual ~TStarTower();

    float X() {return _X;}
    float Y() {return _Y;}
    float Z() {return _Z;}
    float E() {return _E;}
    float RawE() {return _RawE;}
    unsigned int Index() {return _Index;}
    unsigned int ADC() {return _ADC;}
    unsigned int NMatchedTracks() {return _NMatchedTracks;}
    
    float mass = 0.13957;
    void SetMass(float m){mass = m;}

    float P(){return std::sqrt(_E*_E - mass*mass);} 
    float Px(); 
    float Py(); 
    float Pz(); 
    float Pt() {return std::sqrt(Px()*Px() + Py()*Py());} 
    float Theta() {return std::atan2(std::sqrt(_X*_X + _Y*_Y), std::abs(_Z));}
    float Eta() {return -1.0*std::log(std::tan(0.5*Theta()));}
    float Phi();// \phi \in [0, 2\pi]
    float Phi_Std(){return std::atan2(_Y, _X);} // \phi \in [-\pi, \pi] 

    void SetIndex(int i) {_Index = i;}
    void SetADC(int adc) {_ADC = adc;}
    void SetE(float e) {_E = e;}
    void SetRawE(float e) {_RawE = e;}
    void SetNMatchedTracks(int n) {_NMatchedTracks = n;}
    void SetPosX(float x) {_X = x;}
    void SetPosY(float y) {_Y = y;}
    void SetPosZ(float z) {_Z = z;}
    inline void SetPosXYZ(float x, float y, float z) {_X = x; _Y = y; _Z = z;}
    inline void SetPosXYZ(TVector3& pos){_X = pos.x(); _Y = pos.y(); _Z = pos.z();}

    unsigned int _Index = 0;
    unsigned int _ADC = 0;
    float _E = 0;
    float _RawE = 0;
    float _X = 0;
    float _Y = 0;
    float _Z = 0;
    unsigned int _NMatchedTracks = 0;

    ClassDef(TStarTower, 1)
};

#endif
#define TStarVector_cxx

#include "TStarVector.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include <cassert>
#include <iostream>

ClassImp(TStarVector);

using namespace std;

TStarVector::TStarVector(int index, short ch){
    _Index = index;
    _Charge = ch;
}

TStarVector::TStarVector(double p1, double p2, double p3, double _e, int index, short ch, unsigned char _inputType){
    _Index = index;
    _Charge = ch;
    setVector(p1, p2, p3, _e, _inputType);
}

TStarVector::TStarVector(TVector3& _mom, double _e, int index, short ch){
    _Index = index;
    _Charge = ch;
    setVector(_mom, _e);
}

TStarVector::TStarVector(TLorentzVector& _mom, int index, short ch){
    _Index = index;
    _Charge = ch;
    setVector(_mom);
}

TStarVector::TStarVector(const TStarVector& v){
    set(v);
}

TStarVector::~TStarVector(){

}

void TStarVector::set(const TStarVector& v){
    _Index = v._Index;
    _JetIndex = v._JetIndex;
    _Charge = v._Charge;
    setVector(v);
}

void TStarVector::setVector(double p1, double p2, double p3, double _e, unsigned char _inputType){
    switch(_inputType){
        case InputType::kPtEtaPhi:
            setPtEtaPhiE(p1, p2, p3, _e); 
        break;
        case InputType::kPxPyPz:
            setPxPyPzE(p1, p2, p3, _e); 
        break;
        default:
            assert((void("TStarVector::setVector: Unknown input type "), false));
        break;
    }
}

void TStarVector::setVector(TVector3& _mom, double _e){
    _Pt = _mom.Pt();
    _Eta = _mom.Eta();
    _Phi = _mom.Phi();
    _set_phi();
    _Px = _mom.Px();
    _Py = _mom.Py();
    _Pz = _mom.Pz();
    _E = _e;
}

void TStarVector::setVector(TLorentzVector& _mom){
    _Pt = _mom.Pt();
    _Eta = _mom.Eta();
    _Phi = _mom.Phi();
    _set_phi();
    _E = _mom.E();
    _Px = _mom.Px();
    _Py = _mom.Py();
    _Pz = _mom.Pz();
}

void TStarVector::setVector(const TStarVector& v){
    _Pt = v._Pt;
    _Eta = v._Eta;
    _Phi = v._Phi; 
    _E = v._E;
    _Px = v._Px;
    _Py = v._Py;
    _Pz = v._Pz;
}

void TStarVector::_set_phi(){
    assert(_Pt >= 0.0);
    if (_Pt == 0.0){
        _Phi = 0.0;
    }else{
        if(_Phi < 0.0) _Phi += 2.0*TMath::Pi();
        else if(_Phi > 2.0*TMath::Pi()) _Phi -= 2.0*TMath::Pi();
    } 
}

double TStarVector::_force_phi_02pi(double _phi){
    if (_phi < 0.0) return _phi += 2.0*TMath::Pi();
    else if (_phi > 2.0*TMath::Pi()) return _phi -= 2.0*TMath::Pi();
    return _phi;
}

void TStarVector::setPxPyPz() {
    assert(_Pt >= 0.0);
    _Px = _Pt * cos(_Phi);
    _Py = _Pt * sin(_Phi);
    _Pz = _Pt * sinh(_Eta);
}

void TStarVector::setPtEtaPhi() {
    _Pt = sqrt(_Px * _Px + _Py * _Py);
    double _P = sqrt(_Pt * _Pt + _Pz * _Pz);
    _Eta = log(_Pt/fabs(_P - _Pz));
    _Phi = atan2(_Py, _Px);
    _set_phi();
}

void TStarVector::setPxPyPzE(double _px, double _py, double _pz, double _e) {
    assert(_e >= 0.0);
    _Px = _px; 
    _Py = _py; 
    _Pz = _pz; 
    _E = _e;
    setPtEtaPhi();
}

void TStarVector::setPxPyPzM(double _px, double _py, double _pz, double _m) {
    assert(_m >= 0.0);
    double _e = sqrt(_px * _px + _py * _py + _pz * _pz + _m * _m);
    setPxPyPzE(_px, _py, _pz, _e);
}



void TStarVector::setPtEtaPhiE(double _pt, double _eta, double _phi, double _e){
    assert(_pt >= 0.0 && _e >= 0.0);
    _Pt = _pt;
    _Eta = _eta;
    _Phi = _phi;
    _E = _e;
    _set_phi();
    setPxPyPz();
}

void TStarVector::setPtEtaPhiM(double _pt, double _eta, double _phi, double _m){
    double _p2 = _pt * _pt * cosh(_eta) * cosh(_eta);
    setPtEtaPhiE(_pt, _eta, _phi, sqrt(_p2 + _m * _m));
}

void TStarVector::setEtaPhiEM(double _eta, double _phi, double _e, double _m){
    double _p2 = _e * _e - _m * _m;
    if(_p2 < 0){
        cout<<"Tachyonic mass provided! setting it to zero..."<<endl;
        _p2 = _e * _e;
    }
    setPtEtaPhiE(sqrt(_p2)/cosh(_eta), _eta, _phi, _e);
}

void TStarVector::setEtaPhiEM(TVector3& _v, double _e, double _m){
    double _p2 = _e * _e - _m * _m;
    if(_p2 < 0){
        cout<<"Tachyonic mass provided! setting it to zero..."<<endl;
        _p2 = _e * _e;
    }
    setPtEtaPhiE(sqrt(_p2)/cosh(_v.Eta()), _v.Eta(), _v.Phi(), _e);
}

void TStarVector::Print(Option_t *) const {
    TObject::Print();
    cout<<"Index: "<<_Index<<" JetIndex: "<<_JetIndex<<" Charge: "<<_Charge<<endl;
    cout<<"Pt: "<<_Pt<<" Eta: "<<_Eta<<" Phi: "<<_Phi<<"E: "<<_E<<endl;
    cout<<"Px: "<<_Px<<" Py: "<<_Py<<" Pz: "<<_Pz<<endl;
    cout<<"sqrt(Px*Px + Py*Py) = "<<sqrt(_Px*_Px + _Py*_Py)<<" log(Pt/|P + Pz|) = "
    <<log(_Pt/fabs(sqrt(_Px*_Px + _Py*_Py + _Pz*_Pz) - _Pz))<<" atan2(Py/Px) = "<<atan2(_Py, _Px)<<endl;
}

 
double TStarVector::operator()(unsigned char i) const {
    //_set_px_py_pz();
    switch(i) {
        case X:
            return _Px;
        case Y:
            return _Py;
        case Z:
            return _Pz;
        case T:
            return _E;
        default:
            cout << "vector subscripting: bad index (" << i << ")" << endl;
    }
    return 0.;
}






#define TStarVector_cxx

#include "TStarVector.h"
#include "TVector3.h"

#include <cassert>
#include <iostream>

ClassImp(TStarVector);

using namespace std;

TStarVector::TStarVector(){

}

TStarVector::TStarVector(double _pt, double _eta, double _phi, double _e){
    _Pt = _pt;
    _Eta = _eta;
    _Phi = _phi;
    _set_phi();
    _set_px_py_pz();
    _E = _e;
}

TStarVector::TStarVector(TVector3& _mom, double _e){
    _Pt = _mom.Pt();
    _Eta = _mom.Eta();
    _Phi = _mom.Phi();
    _set_phi();
    _Px = _mom.Px();
    _Py = _mom.Py();
    _Pz = _mom.Pz();
    _set_px_py_pz();
    _E = _e;
}

TStarVector::TStarVector(const TStarVector& v){
    _Pt = v._Pt;
    _Eta = v._Eta;
    _Phi = v._Phi; 
    _E = v._E;
    _Px = v._Px;
    _Py = v._Py;
    _Pz = v._Pz;
}

TStarVector::~TStarVector(){

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

void TStarVector::_set_px_py_pz() {
    double prec = 1e-5;
    assert(_Pt >= 0.0);

    if((fabs(_Px) < prec) && (_Pt > prec)) 
        _Px = _Pt * cos(_Phi);
    if((fabs(_Py) < prec) && (_Pt > prec)) 
        _Py = _Pt * sin(_Phi);
    if((fabs(_Pz) < prec) && ((_Pt > prec) && (fabs(_Eta) > prec))) 
        _Pz = _Pt * sinh(_Eta);

    assert(fabs(_Pt - sqrt(_Px * _Px + _Py * _Py)) < prec);
}

void TStarVector::setPxPyPz(double _px, double _py, double _pz) {
    _Px = _px; 
    _Py = _py; 
    _Pz = _pz; 
    _set_px_py_pz();
}

void TStarVector::setPtEtaPhiE(double _pt, double _eta, double _phi, double _e){
    _Pt = _pt;
    _Eta = _eta;
    _Phi = _phi;
    _E = _e;
    _set_phi();
    _set_px_py_pz();
}

void TStarVector::setPtEtaPhiM(double _pt, double _eta, double _phi, double _m){
    setPtEtaPhiE(_pt, _eta, _phi, sqrt(p2() + _m * _m));
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

void TStarVector::print(){
    cout<<"Pt: "<<_Pt<<" Eta: "<<_Eta<<" Phi: "<<_Phi<<"E: "<<_E<<endl;
    cout<<"Px: "<<_Px<<" Py: "<<_Py<<" Pz: "<<_Pz<<endl;
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






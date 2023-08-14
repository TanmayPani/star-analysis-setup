#ifndef TStarVector_h
#define TStarVector_h

#include "TObject.h"
#include "TMath.h"
//#include "TVector3.h"

class TVector3;
class TLorentzVector;

class TStarVector : public TObject{
public:
    enum {
        X = 0,
        Y = 1,
        Z = 2,
        T = 3
    };

    enum InputType {kPtEtaPhi = 0, kPxPyPz = 1};

    TStarVector(int index = -1, short ch = -99);
    TStarVector(double _p1, double _p2, double _p3, double _e, int index = -1, short ch = -99, unsigned char _inputType = InputType::kPtEtaPhi);
    TStarVector(TVector3& _v, double _e, int index = -1, short ch = -99);
    TStarVector(TLorentzVector& _v, int index = -1, short ch = -99);
    TStarVector(const TStarVector& v);

    virtual ~TStarVector();

    int index() const {return _Index;}
    int jetIndex() const {return _JetIndex;}
    short charge() const {return _Charge;}

    double pt() const {
        if(_Pt < 0) return std::sqrt(_Px * _Px + _Py * _Py);
        return _Pt;
    }
    double eta() const {
        if(_Pt < 0 && _Eta < -500) return std::log(fabs(_E + _Pz) / pt());
        return _Eta;
    }
    double phi() {
        if(_Phi < 0) return _force_phi_02pi(std::atan2(_Py, _Px));
        return _Phi;
    }

    double px() const {return _Px;}
    double py() const {return _Py;}
    double pz() const {return _Pz;}
    double energy() const {return _E;}

    double p2()  {return pt() * pt() + _Pz * _Pz;}    
    double p()  {return std::sqrt(p2());}
    double et() {return _E * pt()/p();}

    double pt2() {return pt() * pt();}
    double et2() {return std::pow(et(), 2);}
    double m2() {return _E * _E - p2();}
    double mt2() const {return _E * _E - _Pz * _Pz;}
    double rap() const {return std::log(fabs(_E + _Pz) / std::sqrt(mt2()));}

    virtual void set(const TStarVector& v);

    void setIndex(int index) { _Index = index; }
    void setJetIndex(int index) { _JetIndex = index; }
    void setCharge(short ch) { _Charge = ch; }

    void setPx(double _px) { _Px = _px; }
    void setPy(double _py) { _Py = _py; }
    void setPz(double _pz) { _Pz = _pz; }
    void setPt(double _pt) { _Pt = _pt; }
    void setEta(double _eta) { _Eta = _eta; }
    void setPhi(double _phi) { _Phi = _phi; _set_phi(); }
    void setE (double _e) { _E = _e; }
    void setMass(double _m) { _E = std::sqrt(p2() + _m * _m); }

    void setVector(double _p1, double _p2, double _p3, double _e, unsigned char _inputType = InputType::kPtEtaPhi);
    void setVector(TVector3& _v, double _e);
    void setVector(TLorentzVector& _v);
    void setVector(const TStarVector& v);

    void setPxPyPz();
    void setPxPyPzE(double _px, double _py, double _pz, double _e);
    void setPxPyPzM(double _px, double _py, double _pz, double _m);

    void setPtEtaPhi();
    void setPtEtaPhiE(double _pt, double _eta, double _phi, double _e);
    void setPtEtaPhiM(double _pt, double _eta, double _phi, double _m);

    void setEtaPhiEM(double _eta, double _phi, double _e, double _m);
    void setEtaPhiEM(TVector3& _vec, double _e, double _m);

    virtual void Print (Option_t * option = "")	const;

    double operator() (unsigned char i) const;
    double operator[] (unsigned char i) const {return (*this)(i);}

//protected:
    int _Index = -1;
    int _JetIndex = -1;
    short _Charge = -99;
    double _E = 0;
    double _Px = 0.0;//! 
    double _Py = 0.0;//!   
    double _Pz = 0.0;//! 
    double _Pt = -999; 
    double _Eta = -999;
    double _Phi = -999;

private:
    void _set_phi();
    double _force_phi_02pi(double _phi);
//    bool pxpypz_set = false; //!

    ClassDef(TStarVector, 2)
};


#endif
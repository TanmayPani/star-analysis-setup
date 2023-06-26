#ifndef TStarVector_h
#define TStarVector_h

#include "TObject.h"
#include "TMath.h"
//#include "TVector3.h"

class TVector3;

class TStarVector : public TObject{
public:
    enum {
        X = 0,
        Y = 1,
        Z = 2,
        T = 3
    };

    TStarVector();
    TStarVector(double _pt, double _eta, double _phi, double _e);
    TStarVector(TVector3& _v, double _e);
    TStarVector(const TStarVector& v);

    virtual ~TStarVector();

    double pt() {return _Pt;}
    double eta() {return _Eta;}
    double phi() {return _Phi;}

    double px() {return _Px;}
    double py() {return _Py;}
    double pz() {return _Pz;}

    double p()  {return _Pt * std::cosh(_Eta);}
    double energy() {return _E;}
    double et() {return _E/std::cosh(_Eta);}

    double pt2() {return std::pow(pt(), 2);}
    double p2()  {return std::pow(p(), 2);}    
    double et2() {return std::pow(et(), 2);}
    double m2() {return _E * _E - p2();}
    double rap() {return std::log((_E + _Pz) / _Pt);}

    void setPt(double _pt) { _Pt = _pt; }
    void setEta(double _eta) { _Eta = _eta; }
    void setPhi(double _phi) { _Phi = _phi; _set_phi(); }
    void setE (double _e) { _E = _e; }
    void setMass(double _m) { _E = std::sqrt(p2() + _m * _m); }

    void setPxPyPz(){_set_px_py_pz();}
    void setPxPyPz(double _px, double _py, double _pz);

    void setPtEtaPhiE(double _pt, double _eta, double _phi, double _e);
    void setPtEtaPhiM(double _pt, double _eta, double _phi, double _m);

    void setEtaPhiEM(double _eta, double _phi, double _e, double _m);
    void setEtaPhiEM(TVector3& _vec, double _e, double _m);

    virtual void print();

    double operator() (unsigned char i) const;
    double operator[] (unsigned char i) const {return (*this)(i);}

//protected:
    double _E = 0;
    double _Pt = -999;
    double _Eta = -999;
    double _Phi = -999;
    double _Px = 0.0; //!
    double _Py = 0.0; //!  
    double _Pz = 0.0; //!

private:
    void _set_phi();
    void _set_px_py_pz();

    ClassDef(TStarVector, 2)
};


#endif
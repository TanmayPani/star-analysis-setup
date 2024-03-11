#ifndef TStarMixedEventTrack_H
#define TStarMixedEventTrack_H

#include "TObject.h"
#include "TMath.h"

class TStarMixedEventTrack : public TObject{
public:
    TStarMixedEventTrack();
    TStarMixedEventTrack(double _pt, double _eta, double _phi, short ch = -99);

    void setPt(double _pt){_Pt = _pt;}
    void setEta(double _eta){_Eta = _eta;}
    void setPhi(double _phi){_Phi = _phi; _force_phi_02pi();}
    void setPtEtaPhi(double _pt, double _eta, double _phi){
        _Pt = _pt; _Eta = _eta;
        _Phi = _phi;_force_phi_02pi();
        }

    void setCharge(short ch){_Charge = ch;}
    void setTrackingEfficiency(double eff){_trackingEff = eff;}

    double pt() const {return _Pt;}
    double eta() const {return _Eta;}
    double phi() const {return _Phi;}
    short charge() const {return _Charge;}
    double trackingEff() const {return _trackingEff;}


    double _Pt;
    double _Eta;
    double _Phi;
    short _Charge;
    double _trackingEff = 1.0;

private:
    void _force_phi_02pi();

    ClassDef(TStarMixedEventTrack, 1);

};

#endif // TStarMixedEventTrack_H
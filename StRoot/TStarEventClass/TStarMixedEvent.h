#ifndef TStarMixedEvent_H
#define TStarMixedEvent_H

#include "TObject.h"

class TVector3;
class TStarEvent;

class TStarMixedEvent : public TObject{
public:
    TStarMixedEvent();
    TStarMixedEvent(unsigned int runid, unsigned int eventid);
    TStarMixedEvent(const TStarMixedEvent& ev);
    TStarMixedEvent(const TStarEvent& ev);
    virtual ~TStarMixedEvent();

    unsigned int runNumber() const {return _RunID;}
    unsigned int eventNumber() const {return _EventID;}
    unsigned int gRefMult() const {return _gRefMult;}
    unsigned int refMult() const {return _RefMult;}
    double refMultCorr() const {return _RefMultCorr;}
    unsigned int centrality() const {return _Centrality;}
    double refmultWeight() const {return _RefMultWeight;}
    double reWeight_MB05to30() const {return _MB05to30ReWeight;}
    double weight() const {return _RefMultWeight * _MB05to30ReWeight;}
    double Vz() const {return _pVtx_Z;}
    double Vr() const {return _pVtx_r;}
    double VPD_Vz() const {return _VPD_Vz;}
    double ZDC_Coincidence() const {return _ZDCxx;}
    double BBC_Coincidence() const {return _BBCxx;}
    bool isMB05() const {return _isMB05;}
    bool isMB30() const {return _isMB30;}


    void setEvent(const TStarMixedEvent& ev);
    void setEvent(const TStarEvent& ev);

    void setIdNumbers(unsigned int runid, unsigned int eventid){_RunID = runid; _EventID = eventid;}
    void setRefMults(int gref, int ref){_gRefMult = gref; _RefMult = ref;}
    void setZDCCoincidence(double zdcx){_ZDCxx = zdcx;}
    void setBBCCoincidence(double bbcx){_BBCxx = bbcx;}
    void setPrimaryVertex(TVector3& p);
    void setVPDVz(double vz){_VPD_Vz = vz;}
    //void SetCentrality(int ref16){_Centrality = (ref16 >= 0 && ref16 < 16) ? std::ceil(2.5*(2*ref16 + 1)) : 0;}
    void setCentrality(double cent){_Centrality = cent;}
    void setCorrectedRefmult(double rfcorr){_RefMultCorr = rfcorr;}
    void setRefMultWeight(double wt){_RefMultWeight = wt;}
    void setReWeight_MB05to30(double wt){_MB05to30ReWeight = wt;}
    void setMB05(bool isMB){_isMB05 = isMB;}
    void setMB30(bool isMB){_isMB30 = isMB;}
    
    unsigned int   _RunID         = 0;
    unsigned int   _EventID       = 0;
    unsigned int   _gRefMult      = 0;
    unsigned int   _RefMult       = 0;
    double          _RefMultCorr   = 0;
    double          _Centrality    = 0;
    double          _RefMultWeight = 1.0;
    double          _MB05to30ReWeight = 1.0;
    double          _pVtx_Z        = -999;
    double          _pVtx_r        = -99; //!
    double          _VPD_Vz        = -999; //!
    double          _ZDCxx         = 0; //!
    double          _BBCxx         = 0; //!
    bool            _isMB05        = false; 
    bool            _isMB30        = false;

    ClassDef(TStarMixedEvent, 1)
};

#endif
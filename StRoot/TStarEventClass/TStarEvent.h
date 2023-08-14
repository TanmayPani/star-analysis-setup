#ifndef TStarEvent_h
#define TStarEvent_h

#include "TObject.h"

#include <map>
#include <cmath>
#include <vector>
#include <algorithm>

class TVector3;

class TStarEvent : public TObject{
public:
    TStarEvent();
    TStarEvent(unsigned int runid, unsigned int eventid);
    TStarEvent(const TStarEvent& ev);
    virtual ~TStarEvent();

    unsigned int runNumber() const {return _RunID;}
    unsigned int eventNumber() const {return _EventID;}

    unsigned int gRefMult() const {return _gRefMult;}
    unsigned int refMult() const {return _RefMult;}
    double refMultCorr() const {return _RefMultCorr;}
    unsigned int centrality() const {return _Centrality;}
    double refmultWeight() const {return _RefMultWeight;}
    double genLevelWeight() const {return _GenLevelWeight;}
    double weight() const {return _RefMultWeight * _GenLevelWeight;}

    bool isTriggered(unsigned int trig) const ;
    bool isTriggered(std::string trig) const ;

    bool isMBmon() const {return isTriggered("MBmon");}
    bool isMB5() const {return isTriggered("VPDMB5");}
    bool isVPDMB() const {return isTriggered("VPDMB");}
    bool isMB30() const {return isTriggered("VPDMB30");}
    bool isMB() const {return (isMBmon() || isMB5() || isMB30());}

    bool isHT1() const {return isTriggered("HT1");}
    bool isHT2() const {return isTriggered("HT2");}
    bool isHT3() const {return isTriggered("HT3");}
    bool isHT() const {return (isHT1() || isHT2() || isHT3());}

    double Vz() const {return _pVtx_Z;}
    double Vr() const {return _pVtx_r;}
    double VPD_Vz() const {return _VPD_Vz;}
    double ZDC_Coincidence() const {return _ZDCxx;}
    double BBC_Coincidence() const {return _BBCxx;}

    double rho() const {return _Rho;}
    double sigma() const {return _Sigma;}

    double maxTrackPt() const {return _MaxTrackPt;}
    double maxGenTrackPt() const {return _MaxGenTrackPt;}
    double maxTowerEt() const {return _MaxTowerEt;}
    double maxJetPt() const {return _MaxJetPt;}
    double maxGenJetPt() const {return _MaxGenJetPt;}

    static std::string& runFlag() {return _RunFlag;}

//Modifiers
    static void setRunFlag(unsigned int flag){_RunFlag = "Run" + std::to_string(flag);}
    void setEvent(const TStarEvent& ev);

    void setIdNumbers(unsigned int runid, unsigned int eventid){_RunID = runid; _EventID = eventid;}
    void setRefMults(int gref, int ref){_gRefMult = gref; _RefMult = ref;}
    void setZDCCoincidence(double zdcx){_ZDCxx = zdcx;}
    void setBBCCoincidence(double bbcx){_BBCxx = bbcx;}
    void setPrimaryVertex(TVector3& p);
    void setVPDVz(double vz){_VPD_Vz = vz;}
 
    //void SetCentrality(int ref16){_Centrality = (ref16 >= 0 && ref16 < 16) ? std::ceil(2.5*(2*ref16 + 1)) : 0;}
    void setCentrality(double cent){_Centrality = cent;}

    void setTriggers(std::vector<unsigned int>& trigs);

    void setCorrectedRefmult(double rfcorr){_RefMultCorr = rfcorr;}
    void setRefMultWeight(double wt){_RefMultWeight = wt;}
    void setGenLevelWeight(double wt){_GenLevelWeight = wt;}

    void setRho(double rho){_Rho = rho;}
    void setSigma(double sigma){_Sigma = sigma;}

    void setMaxTrackPt(double max){_MaxTrackPt = max;}
    void setMaxGenTrackPt(double max){_MaxGenTrackPt = max;}
    void setMaxTowerEt(double max){_MaxTowerEt = max;}
    void setMaxJetPt(double max){_MaxJetPt = max;}
    void setMaxGenJetPt(double max){_MaxGenJetPt = max;}

    void addTrigger(unsigned int trig){_Triggers.push_back(trig);}

    virtual void Print(Option_t *option = "") const;

    unsigned int   _RunID         = 0;//!
    unsigned int   _EventID       = 0;//!
    unsigned int   _gRefMult      = 0;//!
    unsigned int   _RefMult       = 0;//!
    double          _RefMultCorr   = 0;
    double          _Centrality    = 0;
    double          _RefMultWeight = 1.0;
    double          _GenLevelWeight = 1.0;//!
    std::vector<unsigned int> _Triggers;
    double          _pVtx_Z        = -999;
    double          _pVtx_r        = -99; //!
    double          _VPD_Vz        = -999; //!
    double          _ZDCxx         = 0; //!
    double          _BBCxx         = 0; //!
    double          _Rho           = 0; //!
    double          _Sigma         = 0; //!
    double          _MaxTrackPt    = 0;
    double          _MaxGenTrackPt = 0;//!
    double          _MaxTowerEt    = 0;
    double          _MaxJetPt      = 0;
    double          _MaxGenJetPt   = 0;//!

private:
    static std::string _RunFlag; 
    static std::map<std::string, std::vector<unsigned int>> _triggerMap; //!

    ClassDef(TStarEvent, 3)
};
#endif
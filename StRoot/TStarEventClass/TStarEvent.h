#ifndef TStarEvent_h
#define TStarEvent_h

#include "TObject.h"

//#include <set>
#include <cmath>
#include <vector>
#include <algorithm>

class TVector3;
class TClonesArray;
class TStarTrack;
class TStarTower;

class TStarEvent : public TObject{
public:
    TStarEvent();
    TStarEvent(unsigned int runid, unsigned int eventid);
    TStarEvent(const TStarEvent& ev);
    virtual ~TStarEvent();

    unsigned int runNumber(){return _RunID;}
    unsigned int eventNumber(){return _EventID;}

    unsigned int gRefMult(){return _gRefMult;}
    unsigned int refMult(){return _RefMult;}
    double refMultCorr(){return _RefMultCorr;}
    unsigned int centrality(){return _Centrality;}
    double weight(){return _Weight;}

    bool isTriggered(unsigned int trig){return (std::find(_Triggers.begin(), _Triggers.end(), trig) != _Triggers.end());}

    bool isMBmon(){return (isTriggered(450011) || isTriggered(450021));}
    bool isMB5(){return (isTriggered(450005) || isTriggered(450008) || isTriggered(450009) || isTriggered(450014) || isTriggered(450015) ||
                        isTriggered(450018) || isTriggered(450024) || isTriggered(450025) || isTriggered(450050) || isTriggered(450060));}
    bool isMB30(){return (isTriggered(450010) || isTriggered(450020));}
    bool isMB(){return (isMBmon() || isMB5() || isMB30());}

    bool isHT1(){return (isTriggered(450201) || isTriggered(450211));}
    bool isHT2(){return (isTriggered(450202) || isTriggered(450212));}
    bool isHT3(){return (isTriggered(450203) || isTriggered(450213));}
    bool isHT(){return (isHT1() || isHT2() || isHT3());}

    double Vz(){return _pVtx_Z;}
    double Vr(){return _pVtx_r;}
    double VPD_Vz(){return _VPD_Vz;}
    double ZDC_Coincidence(){return _ZDCxx;}
    double BBC_Coincidence(){return _BBCxx;}
//    unsigned short NumberOfGlobalTracks(){return _NGlobalTracks;}
    TClonesArray* getTracks(){return Tracks;}
    TClonesArray* getTowers(){return Towers;} 
    double maxTrackPt(){return _MaxTrackPt;}
    double maxTowerEt(){return _MaxTowerEt;}

//Modifiers

    void setEvent(const TStarEvent& ev);

    void setIdNumbers(unsigned int runid, unsigned int eventid){_RunID = runid; _EventID = eventid;}
    void setRefMults(int gref, int ref){_gRefMult = gref; _RefMult = ref;}
    void setZDCCoincidence(double zdcx){_ZDCxx = zdcx;}
    void setBBCCoincidence(double bbcx){_BBCxx = bbcx;}
    void setPrimaryVertex(TVector3& p);
    void setVPDVz(double vz){_VPD_Vz = vz;}
 
    //void SetCentrality(int ref16){_Centrality = (ref16 >= 0 && ref16 < 16) ? std::ceil(2.5*(2*ref16 + 1)) : 0;}
    void setCentrality(double cent){_Centrality = cent;}

    void setTriggers(std::vector<unsigned int>& trigs){_Triggers = trigs;}

    void setCorrectedRefmult(double rfcorr){_RefMultCorr = rfcorr;}
    void setWeight(double wt){_Weight = wt;}

    void addTrigger(unsigned char trig){_Triggers.push_back(trig);}

    TStarTrack* addTrack();
    TStarTower* addTower();
    void addTrack(const TStarTrack& trk);
    void addTower(const TStarTower& tow);

    void clearTrackArray();
    void clearTowerArray();
    void setMaxTrackPt(double max){_MaxTrackPt = max;}
    void setMaxTowerEt(double max){_MaxTowerEt = max;}

    void clearEvent();

    void print();

    unsigned int   _RunID         = 0;
    unsigned int   _EventID       = 0;
    unsigned int   _gRefMult      = 0;
    unsigned int   _RefMult       = 0;
    double          _RefMultCorr   = 0;
    double           _Centrality    = 0;
    double          _Weight        = 1.0;
    std::vector<unsigned int> _Triggers;
    double          _pVtx_Z        = -999;
    double          _pVtx_r        = -99;
    double          _VPD_Vz        = -999;
    double          _ZDCxx         = 0; //!
    double          _BBCxx         = 0; //!
    double          _MaxTrackPt    = 0;
    double          _MaxTowerEt    = 0;

    TClonesArray *Tracks;
    TClonesArray *Towers;

    ClassDef(TStarEvent, 3)
};
#endif
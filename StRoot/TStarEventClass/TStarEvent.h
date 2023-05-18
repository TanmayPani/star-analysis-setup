#ifndef TStarEvent_h
#define TStarEvent_h

#include "TObject.h"

class TVector3;
class TClonesArray;
class TStarTrack;
class TStarTower;

class TStarEvent : public TObject{
public:
    TStarEvent();
    TStarEvent(unsigned int runid, unsigned int eventid);
    virtual ~TStarEvent();

    unsigned int RunNumber(){return _RunID;}
    unsigned int EventNumber(){return _EventID;}

    unsigned int gRefMult(){return _gRefMult;}
    unsigned int RefMult(){return _RefMult;}
    float RefMultCorr(){return _RefMultCorr;}
    unsigned char Centrality(){return _Centrality;}
    float Weight(){return _Weight;}

    bool IsMB(){return (_IsMB || _IsMB5 || _IsMB30);}
    bool IsMB5(){return _IsMB5;}
    bool IsMB30(){return _IsMB30;}

    bool IsHT1(){return _IsHT1;}
    bool IsHT2(){return _IsHT2;}
    bool IsHT3(){return _IsHT3;}
    bool IsHT(){return (_IsHT1 || _IsHT2 || _IsHT2);}

    float Vz(){return _pVtx_Z;}
    float Vr(){return _pVtx_r;}
    float VPD_Vz(){return _VPD_Vz;}
    float ZDC_Coincidence(){return _ZDCxx;}
    float BBC_Coincidence(){return _BBCxx;}
    unsigned short NumberOfGlobalTracks(){return _NGlobalTracks;}
    TClonesArray* GetTracks(){return Tracks;}
    TClonesArray* GetTowers(){return Towers;} 
    float MaxTrackPt(){return _MaxTrackPt;}
    float MaxTowerEt(){return _MaxTowerEt;}

    void SetIdNumbers(unsigned int runid, unsigned int eventid){_RunID = runid; _EventID = eventid;}
    void SetRefMults(int gref, int ref){_gRefMult = gref; _RefMult = ref;}
    void SetZDCCoincidence(float zdcx){_ZDCxx = zdcx;}
    void SetBBCCoincidence(float bbcx){_BBCxx = bbcx;}
    void SetPrimaryVertex(TVector3& p);
    void SetVPDVz(float vz){_VPD_Vz = vz;}
 
    void SetCentrality(int cent){_Centrality = (cent >= 1 && cent <= 16) ? cent : 0;}
    void SetCorrectedRefmult(float rfcorr){_RefMultCorr = rfcorr;}
    void SetWeight(float wt){_Weight = wt;}

    void SetMBStatus(bool b){_IsMB = b;}
    void SetMB5Status(bool b){_IsMB5 = b;}
    void SetMB30Status(bool b){_IsMB30 = b;}
    void SetHT1Status(bool b){_IsHT1 = b;}
    void SetHT2Status(bool b){_IsHT2 = b;}
    void SetHT3Status(bool b){_IsHT3 = b;}

    void SetNumberOfGlobalTracks(unsigned short n){_NGlobalTracks = n;}
    TStarTrack* AddTrack();
    TStarTower* AddTower();
    void ClearTrackArray();
    void ClearTowerArray();
    void SetMaxTrackPt(float max){_MaxTrackPt = max;}
    void SetMaxTowerEt(float max){_MaxTowerEt = max;}

    unsigned int _RunID = 0;
    unsigned int _EventID = 0;
    unsigned int _gRefMult = 0;
    unsigned int _RefMult = 0;
    float _RefMultCorr = 0;
    unsigned char _Centrality = 0;
    float _Weight = 1.0;

    bool _IsMB = false;
    bool _IsMB5 = false;
    bool _IsMB30 = false;

    bool _IsHT1 = false;
    bool _IsHT2 = false;
    bool _IsHT3 = false;

    float _pVtx_Z = -999;
    float _pVtx_r = -99;
    float _VPD_Vz = -999;
    float _ZDCxx = 0;
    float _BBCxx = 0;

    unsigned short _NGlobalTracks = 0;

    float _MaxTrackPt = 0;
    float _MaxTowerEt = 0;

    TClonesArray *Tracks;
    TClonesArray *Towers;

    ClassDef(TStarEvent, 2)
};
#endif
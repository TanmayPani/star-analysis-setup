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
    TStarEvent(int runid, int eventid);
    virtual ~TStarEvent();

    int RunNumber(){return _RunID;}
    int EventNumber(){return _EventID;}

    int gRefMult(){return _gRefMult;}
    int RefMult(){return _RefMult;}
    double RefMultCorr(){return _RefMultCorr;}
    double Centrality(){return _Centrality;}
    double Peripheral_Reweight(){return _Peripheral_ReWeight;}

    double MB5toMB30_ReWeight(){return _MB5toMB30_ReWeight;}
    bool IsMB(){return (_IsMB || _IsMB5 || _IsMB30);}
    bool IsMB5(){return _IsMB5;}
    bool IsMB30(){return _IsMB30;}

    bool IsHT1(){return _IsHT1;}
    bool IsHT2(){return _IsHT2;}
    bool IsHT3(){return _IsHT3;}
    bool IsHT(){return (_IsHT1 || _IsHT2 || _IsHT2);}

    double Vz(){return _pVtx_Z;}
    double Vr(){return _pVtx_r;}
    double VPD_Vz(){return _VPD_Vz;}
    double ZDC_Coincidence(){return _ZDCxx;}
    double BBC_Coincidence(){return _BBCxx;}
    double NumberOfEmcTriggers(){return _NTriggers;}
    double NumberOfGlobalTracks(){return _NGlobalTracks;}
    double NumberOfTowers(){return _NTowers;}
    TClonesArray* GetTracks(){return Tracks;}
    TClonesArray* GetTowers(){return Towers;} 
    double MaxTrackPt(){return _MaxTrackPt;}
    double MaxTowerEt(){return _MaxTowerEt;}

    void SetIdNumbers(int runid, int eventid){_RunID = runid; _EventID = eventid;}
    void SetRefMults(int gref, int ref){_gRefMult = gref; _RefMult = ref;}
    void SetZDCCoincidence(double zdcx){_ZDCxx = zdcx;}
    void SetBBCCoincidence(double bbcx){_BBCxx = bbcx;}
    void SetPrimaryVertex(TVector3& p);
    void SetVPDVz(double vz){_VPD_Vz = vz;}
 
    void SetCentrality(double cent){_Centrality = cent;}
    void SetCorrectedRefmult(double rfcorr){_RefMultCorr = rfcorr;}
    void SetPeripheralReweight(double wt){_Peripheral_ReWeight = wt;}
    void SetMB5toMB30Reweight(double wt){_MB5toMB30_ReWeight = wt;}

    void SetMBStatus(bool b){_IsMB = b;}
    void SetMB5Status(bool b){_IsMB5 = b;}
    void SetMB30Status(bool b){_IsMB30 = b;}
    void SetHT1Status(bool b){_IsHT1 = b;}
    void SetHT2Status(bool b){_IsHT2 = b;}
    void SetHT3Status(bool b){_IsHT3 = b;}

    void SetNumberOfGlobalTracks(int n){_NGlobalTracks = n;}
    void SetNumberOfTowers(int n){_NTowers = n;}
    void SetNumberOfEmcTriggers(int n){_NTriggers = n;}
    TStarTrack* AddTrack();
    TStarTower* AddTower();
    void ClearTrackArray();
    void ClearTowerArray();
    void SetMaxTrackPt(double max){_MaxTrackPt = max;}
    void SetMaxTowerEt(double max){_MaxTowerEt = max;}

    int _RunID = 0;
    int _EventID = 0;
    int _gRefMult = 0;
    int _RefMult = 0;
    double _RefMultCorr = 0;
    double _Centrality = -99;
    double _Peripheral_ReWeight = 1;

    double _MB5toMB30_ReWeight = 0;
    bool _IsMB = false;
    bool _IsMB5 = false;
    bool _IsMB30 = false;

    bool _IsHT1 = false;
    bool _IsHT2 = false;
    bool _IsHT3 = false;

    double _pVtx_Z = -999;
    double _pVtx_r = -99;
    double _VPD_Vz = -999;
    double _ZDCxx = 0;
    double _BBCxx = 0;

    int _NTriggers = 0;
    int _NGlobalTracks = 0;
    int _NTowers = 0;

    double _MaxTrackPt = 0;
    double _MaxTowerEt = 0;

    TClonesArray *Tracks;
    TClonesArray *Towers;

    ClassDef(TStarEvent, 2)
};
#endif
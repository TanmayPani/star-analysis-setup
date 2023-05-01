#ifndef StEventTreeStruct_h
#define StEventTreeStruct_h

#include <iostream>
#include <vector>
#include <map>

#include "TMath.h"
#include "TObject.h"
#include "TVector3.h"

using namespace std;

class MyStTrack : public TObject{
public:
    MyStTrack(){}
    MyStTrack(int i, double pt, double eta, double phi, short ch){
        index = i;
        Pt = pt;
        Eta = eta;
        Phi = phi;
        if(Phi < 0.0) Phi += 2.0*TMath::Pi();
        if(Phi > 2.0*TMath::Pi()) Phi -= 2.0*TMath::Pi();
        Charge = ch;
    }
    MyStTrack(int i, TVector3& t, short ch){
        index = i;
        Pt = t.Perp();
        Eta = t.Eta();
        Phi = t.Phi();
        if(Phi < 0.0) Phi += 2.0*TMath::Pi();
        if(Phi > 2.0*TMath::Pi()) Phi -= 2.0*TMath::Pi();
        Charge = ch;
    }
    virtual ~MyStTrack(){}

    int index = 0;
    double Pt = 0;
    double Eta = -99;
    double Phi = -99;
    double Charge = -99;

    double trackingEff = 0;
    int MatchedTowerIndex = -99;

    ClassDef(MyStTrack, 1);
};

class MyStTower : public TObject{
public:
    MyStTower(){}
    MyStTower(int tid, int tadc, double tE){
        index = tid;
        ADC = tadc;
        Energy = tE;
    } 
    MyStTower(int tid, int tadc, double tE, TVector3& towPos){
        index = tid;
        ADC = tadc;
        Energy = tE;
        Eta = towPos.Eta();
        Phi = towPos.Phi();
        if(Phi < 0.0) Phi += 2.0*TMath::Pi();
        if(Phi > 2.0*TMath::Pi()) Phi -= 2.0*TMath::Pi();
    }
    virtual ~MyStTower(){}

    void SetHighTowerStatus(vector<bool> htstat){
        if(htstat.size()!=4){
            cout<<"Something is wrong!"<<endl;
            return;
        }else{
            IsHT0 = htstat[0];
            IsHT1 = htstat[1];
            IsHT2 = htstat[2];
            IsHT3 = htstat[3];
        }
    }
    int index = 0;
    int ADC = 0;
    double Energy = 0;

    double Eta = -99;
    double Phi = -99;

    bool IsHT0 = false;
    bool IsHT1 = false;
    bool IsHT2 = false;
    bool IsHT3 = false;

    int NMatchedTracks = 0;
    double EnergyCorr = 0; 

    ClassDef(MyStTower, 1);   
};

class MyStEvent : public TObject{
public:
    MyStEvent(){}
    MyStEvent(int runid, int eventid){
        RunID = runid;
        EventID = eventid;
    }
    virtual ~MyStEvent(){}

    void ClearVectors(){
        Tracks.clear();
        Towers.clear();
    }

    void AddTrack(const MyStTrack& t){
        Tracks.push_back(t);
    }

    void AddTower(const MyStTower& t){
        Towers.push_back(t);
    }

    int RunID = 0;
    int EventID = 0;
    int gRefMult = 0;
    int RefMult = 0;
    double RefMultCorr = 0;
    double Centrality = -99;
    double Peripheral_ReWeight = 1;
    double MB5toMB30_ReWeight = 1;

    bool IsMB = false;
    bool IsMB5 = false;
    bool IsMB30 = false;

    bool IsHT0 = false;
    bool IsHT1 = false;
    bool IsHT2 = false;
    bool IsHT3 = false;

    double pVtx_Z = -999;
    double pVtx_r = -99;
    double VPD_Vz = -99;
    double ZDCxx = 0;
    double BBCxx = 0;

    int NTriggers = 0;
    int NTracks = 0;
    int NTowers = 0;

    double MaxTrackPt = 0;
    double MaxTowerEt = 0;

    vector<MyStTrack> Tracks;
    vector<MyStTower> Towers;

    ClassDef(MyStEvent, 2);
};
//#ifdef __CINT__
//#pragma link C++ class MyStTrack+;
//#pragma link C++ class MyStTower+;
//#pragma link C++ class MyStEvent+;
//#endif
#endif


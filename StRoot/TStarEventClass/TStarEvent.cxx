#define TStarEvent_cxx

#include "TStarEvent.h"

#include "TVector3.h"
#include "TLorentzVector.h"
#include "TClonesArray.h"
#include "TStarTrack.h"
#include "TStarTower.h"
#include "TStarGenTrack.h"

#include <iostream>

ClassImp(TStarEvent);

using namespace std;

map<string, vector<unsigned int>> TStarEvent::_triggerMap = {};
//    {"Run14_MBmon", {450011, 450021}}, 
//    {"Run14_VPDMB5", {450005, 450008, 450009, 450014, 450015, 450018, 450024, 450025, 450050, 450060}},
//    {"Run14_VPDMB30", {450010, 450020}}, {"Run14_HT1", {450201, 450211}}, {"Run14_HT2", {450202, 450212}},
//    {"Run14_HT3", {450203, 450213}}, {"Run14_HT", {450201, 450211, 450202, 450212, 450203, 450213}},
//    {"Run12_VPDMB", {370001, 370011}}, {"Run12_HT1", {370511, 370546}}, {"Run12_HT2", {370521, 370522, 370531, 370980}},
//    {"Run12_HT3", {380206, 380216}}
//};

TStarEvent::TStarEvent(){

}

TStarEvent::TStarEvent(unsigned int runid, unsigned int eventid){
    _RunID = runid;
    _EventID = eventid;
}

TStarEvent::TStarEvent(const TStarEvent& ev){setEvent(ev);}

TStarEvent::~TStarEvent(){

}

void TStarEvent::setEvent(const TStarEvent& ev){
    _RunID         =  ev._RunID        ;
    _EventID       =  ev._EventID      ;
    _gRefMult      =  ev._gRefMult     ;
    _RefMult       =  ev._RefMult      ;
    _RefMultCorr   =  ev._RefMultCorr  ;
    _Centrality    =  ev._Centrality   ;
    _RefMultWeight =  ev._RefMultWeight;
    _GenLevelWeight= ev._GenLevelWeight;
    setTriggers(ev._Triggers)          ;
    _pVtx_Z        =  ev._pVtx_Z       ;
    _pVtx_r        =  ev._pVtx_r       ;
    _VPD_Vz        =  ev._VPD_Vz       ;
    _ZDCxx         =  ev._ZDCxx        ;
    _BBCxx         =  ev._BBCxx        ;
    _Rho           =  ev._Rho          ;
    _Sigma         =  ev._Sigma        ;
    _MaxTrackPt    =  ev._MaxTrackPt   ;
    _MaxGenTrackPt =  ev._MaxGenTrackPt;
    _MaxTowerEt    =  ev._MaxTowerEt   ;
    _MaxJetPt      =  ev._MaxJetPt     ;
    _MaxGenJetPt   =  ev._MaxGenJetPt  ;
}

void TStarEvent::setPrimaryVertex(TVector3& pVtx){
    _pVtx_Z = pVtx.z();
    _pVtx_r = pVtx.Perp();
}

bool TStarEvent::isTriggered(unsigned int trig) const {
    return (std::find(_Triggers.begin(), _Triggers.end(), trig) != _Triggers.end());
}

bool TStarEvent::isTriggered(std::string trig) const {
    //trig = "Run"+to_string(runYear()) + "_" + trig;
    if(_triggerMap.find(trig) == _triggerMap.end()) return false;
    bool res = false;
    for(auto t : _triggerMap[trig]) res = res || isTriggered(t);
    return res;
}

void TStarEvent::setTriggers(std::vector<unsigned int> triggers){
   _Triggers.assign(triggers.begin(), triggers.end()); 
}

void TStarEvent::addTriggerToMap(std::string trig, unsigned int* ids, int size){
    vector<unsigned int> idsVec = {};
    cout<<"adding trigger to map: "<<trig<<" with ids: ";
    for(int i = 0; i < size; ++i){
        cout<<ids[i]<<" ";
        idsVec.push_back(ids[i]);
    }cout<<endl;
    _triggerMap[trig] = idsVec;
}
//void TStarEvent::addTriggerToMap(std::string trig, unsigned int id){_triggerMap[trig].push_back(id);}
//void TStarEvent::setTriggerMap(std::map<std::string, std::vector<unsigned int>> trigMap){_triggerMap = trigMap;}
//void TStarEvent::clearTriggerMap(){_triggerMap.clear();}
//vector<unsigned int> TStarEvent::getTriggerIDs(string trig){return _triggerMap[trig];}
//map<string, vector<unsigned int>> TStarEvent::triggerMap(){return _triggerMap;}

void TStarEvent::Print(Option_t* ) const{
    cout<<"********** Event Info from TStarEvent **********"<<endl;
    cout<<"RunID: "<<_RunID<<" EventID: "<<_EventID<<endl;
    cout<<"Gen level weight: "<<_GenLevelWeight<<endl;
    cout<<"gRefMult: "<<_gRefMult<<" RefMult: "<<_RefMult<<" RefMultCorr: "<<_RefMultCorr<<endl;
    cout<<"Centrality: "<<_Centrality<<" Refmult Weight: "<<_RefMultWeight<<endl;
    cout<<"pVtx_Z: "<<_pVtx_Z<<" pVtx_r: "<<_pVtx_r<<"VPD Vz: "<<_VPD_Vz<<endl;
    cout<<"ZDCxx: "<<_ZDCxx<<" BBCxx: "<<_BBCxx<<endl;
    cout<<"MaxTrackPt: "<<_MaxTrackPt<<" MaxTowerEt: "<<_MaxTowerEt<<" MaxGenTrackPt: "<<_MaxGenTrackPt<<endl;
    cout<<"MaxJetPt: "<<_MaxJetPt<<" MaxGenJetPt: "<<_MaxGenJetPt<<endl;
    cout<<"Rho: "<<_Rho<<" Sigma: "<<_Sigma<<endl;

    for(unsigned int i = 0; i < _Triggers.size(); ++i){
        cout<<"Trigger "<<i<<": "<<_Triggers[i]<<endl;
    }
    cout<<"MB_mon: "<<isMBmon()<<endl;
    cout<<"MB30: "<<isMB30()<<endl;
    cout<<"MB5: "<<isMB5()<<endl;
    cout<<"HT1: "<<isHT1()<<endl;
    cout<<"HT2: "<<isHT2()<<endl;
    cout<<"HT3: "<<isHT3()<<endl;

    cout<<"********** End of Event Info **********"<<endl;
}



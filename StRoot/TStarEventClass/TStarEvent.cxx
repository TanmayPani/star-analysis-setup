#define TStarEvent_cxx

#include "TStarEvent.h"

#include "TVector3.h"
#include "TClonesArray.h"
#include "TStarTrack.h"
#include "TStarTower.h"

#include <iostream>

ClassImp(TStarEvent);

using namespace std;

TStarEvent::TStarEvent(){
    Tracks = new TClonesArray("TStarTrack", 1000);
    Towers = new TClonesArray("TStarTower", 4800);

}

TStarEvent::TStarEvent(unsigned int runid, unsigned int eventid){
    _RunID = runid;
    _EventID = eventid;
    Tracks = new TClonesArray("TStarTrack", 1000);
    Towers = new TClonesArray("TStarTower", 4800);
}

TStarEvent::TStarEvent(const TStarEvent& ev){
    _RunID         =  ev._RunID        ;
    _EventID       =  ev._EventID      ;
    _gRefMult      =  ev._gRefMult     ;
    _RefMult       =  ev._RefMult      ;
    _RefMultCorr   =  ev._RefMultCorr  ;
    _Centrality    =  ev._Centrality   ;
    _Weight        =  ev._Weight       ;
    _Triggers      =  ev._Triggers     ;
    _pVtx_Z        =  ev._pVtx_Z       ;
    _pVtx_r        =  ev._pVtx_r       ;
    _VPD_Vz        =  ev._VPD_Vz       ;
    _ZDCxx         =  ev._ZDCxx        ;
    _BBCxx         =  ev._BBCxx        ;
    _MaxTrackPt    =  ev._MaxTrackPt   ;
    _MaxTowerEt    =  ev._MaxTowerEt   ;

    for(int i = 0; i < ev.Tracks->GetEntriesFast(); ++i){
        TStarTrack* _trk = static_cast<TStarTrack*>(ev.Tracks->At(i));
        new((*Tracks)[i]) TStarTrack(*_trk);
    }

    for(int i = 0; i < ev.Towers->GetEntriesFast(); ++i){
        TStarTower* _tow = static_cast<TStarTower*>(ev.Towers->At(i));
        new((*Towers)[i]) TStarTower(*_tow);
    } 

}

TStarEvent::~TStarEvent(){
    if(!TestBit(kIsOnHeap)){
        delete Tracks;
        delete Towers;
    }
}

void TStarEvent::setEvent(const TStarEvent& ev){
    _RunID         =  ev._RunID        ;
    _EventID       =  ev._EventID      ;
    _gRefMult      =  ev._gRefMult     ;
    _RefMult       =  ev._RefMult      ;
    _RefMultCorr   =  ev._RefMultCorr  ;
    _Centrality    =  ev._Centrality   ;
    _Weight        =  ev._Weight       ;
    _Triggers      =  ev._Triggers     ;
    _pVtx_Z        =  ev._pVtx_Z       ;
    _pVtx_r        =  ev._pVtx_r       ;
    _VPD_Vz        =  ev._VPD_Vz       ;
    _ZDCxx         =  ev._ZDCxx        ;
    _BBCxx         =  ev._BBCxx        ;
    _MaxTrackPt    =  ev._MaxTrackPt   ;
    _MaxTowerEt    =  ev._MaxTowerEt   ;
    
    for(int i = 0; i < ev.Tracks->GetEntriesFast(); ++i){
        TStarTrack* _trk = static_cast<TStarTrack*>(ev.Tracks->At(i));
        new((*Tracks)[i]) TStarTrack(*_trk);
    }

    for(int i = 0; i < ev.Towers->GetEntriesFast(); ++i){
        TStarTower* _tow = static_cast<TStarTower*>(ev.Towers->At(i));
        new((*Towers)[i]) TStarTower(*_tow);
    } 
}

void TStarEvent::setPrimaryVertex(TVector3& pVtx){
    _pVtx_Z = pVtx.z();
    _pVtx_r = pVtx.Perp();
}

TStarTrack* TStarEvent::addTrack(){
    int index = Tracks->GetEntriesFast();
    TStarTrack* _track = static_cast<TStarTrack*>(Tracks->ConstructedAt(index));
    return _track;
}

void TStarEvent::addTrack(const TStarTrack& track){
    new((*Tracks)[Tracks->GetEntriesFast()]) TStarTrack(track);
}

TStarTower* TStarEvent::addTower(){
    int index = Towers->GetEntriesFast();
    TStarTower* _tower = static_cast<TStarTower*>(Towers->ConstructedAt(index));
    return _tower;
}

void TStarEvent::addTower(const TStarTower& tower){
    new((*Towers)[Towers->GetEntriesFast()]) TStarTower(tower);
}

void TStarEvent::clearTrackArray(){
    Tracks->Clear();
}

void TStarEvent::clearTowerArray(){
    Towers->Clear();
}

void TStarEvent::clearEvent(){
    Tracks->Clear();
    Towers->Clear();
    _RunID         =  0;
    _EventID       =  0;
    _gRefMult      =  0;
    _RefMult       =  0;
    _RefMultCorr   =  0;
    _Centrality    =  0;
    _Weight        =  0;
    _Triggers.clear();
    _pVtx_Z        =  0;
    _pVtx_r        =  0;
    _VPD_Vz        =  0;
    _ZDCxx         =  0;
    _BBCxx         =  0;
    _MaxTrackPt    =  0;
    _MaxTowerEt    =  0;
}

void TStarEvent::print(){
    cout<<"********** Event Info from TStarEvent **********"<<endl;
    cout<<"RunID: "<<_RunID<<" EventID: "<<_EventID<<endl;
    cout<<"gRefMult: "<<_gRefMult<<" RefMult: "<<_RefMult<<" RefMultCorr: "<<_RefMultCorr<<endl;
    cout<<"Centrality: "<<_Centrality<<" Weight: "<<_Weight<<endl;
    cout<<"pVtx_Z: "<<_pVtx_Z<<" pVtx_r: "<<_pVtx_r<<"VPD Vz: "<<_VPD_Vz<<endl;
    cout<<"ZDCxx: "<<_ZDCxx<<" BBCxx: "<<_BBCxx<<endl;
    cout<<"MaxTrackPt: "<<_MaxTrackPt<<" MaxTowerEt: "<<_MaxTowerEt<<endl;

    for(int i = 0; i < _Triggers.size(); ++i){
        cout<<"Trigger "<<i<<": "<<_Triggers[i]<<endl;
    }
    cout<<"MB_mon: "<<isMBmon()<<endl;
    cout<<"MB30: "<<isMB30()<<endl;
    cout<<"MB5: "<<isMB5()<<endl;
    cout<<"HT1: "<<isHT1()<<endl;
    cout<<"HT2: "<<isHT2()<<endl;
    cout<<"HT3: "<<isHT3()<<endl;
    for(int i = 0; i < Tracks->GetEntriesFast(); ++i){
        TStarTrack* _trk = static_cast<TStarTrack*>(Tracks->At(i));
        _trk->print();
    }

    for(int i = 0; i < Towers->GetEntriesFast(); ++i){
        TStarTower* _tow = static_cast<TStarTower*>(Towers->At(i));
        _tow->print();
    }

    cout<<"********** End of Event Info **********"<<endl;
}



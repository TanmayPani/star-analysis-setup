#define TStarEvent_cxx

#include "TStarEvent.h"

ClassImp(TStarEvent);

TStarEvent::TStarEvent(){
    Tracks = new TClonesArray("TStarTrack", 1000);
    Towers = new TClonesArray("TStarTower", 4800);

}

TStarEvent::TStarEvent(int runid, int eventid){
    _RunID = runid;
    _EventID = eventid;
    Tracks = new TClonesArray("TStarTrack", 1000);
    Towers = new TClonesArray("TStarTower", 4800);
}

TStarEvent::~TStarEvent(){
    if(!TestBit(kIsOnHeap)){
        delete Tracks;
        delete Towers;
    }
}

TStarTrack* TStarEvent::AddTrack(){
    int index = Tracks->GetEntriesFast();
    TStarTrack* _track = static_cast<TStarTrack*>(Tracks->ConstructedAt(index));
    return _track;
}

TStarTower* TStarEvent::AddTower(){
    int index = Towers->GetEntriesFast();
    TStarTower* _tower = static_cast<TStarTower*>(Towers->ConstructedAt(index));
    return _tower;
}

void TStarEvent::SetPrimaryVertex(TVector3& pVtx){
    _pVtx_Z = pVtx.z();
    _pVtx_r = pVtx.Perp();
}

int TStarEvent::SetCentralityDetails(StRefMultCorr *grefmultCorr){
    if(!grefmultCorr){
        _RefMultCorr = _RefMult;
        return -99;
    }else{
        grefmultCorr->init(_RunID);
        grefmultCorr->initEvent(_gRefMult, _pVtx_Z, _ZDCxx);
        _RefMultCorr = grefmultCorr->getRefMultCorr(_gRefMult, _pVtx_Z, _ZDCxx, 2);
        //centbin9 = grefmultCorr->getCentralityBin9();
        int centbin16 = grefmultCorr->getCentralityBin16();
        //ref9 = 8-centbin9;
        //int ref16 = ; 
        _Centrality = 5.0*(15-centbin16);
        _Peripheral_ReWeight = grefmultCorr->getWeight();
        return centbin16;
    }
}

void TStarEvent::SetMB5toMB30Reweight(StRefMultCorr *grefmultCorr){
    if(!grefmultCorr)return;
    else{
        grefmultCorr->init(_RunID);
        grefmultCorr->initEvent(_gRefMult, _pVtx_Z, _ZDCxx); 
        _MB5toMB30_ReWeight = (_IsMB5 && !_IsMB30) ? grefmultCorr->getWeight() : 1.0;
    }
}




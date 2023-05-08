#define TStarEvent_cxx

#include "TStarEvent.h"

#include "TVector3.h"
#include "TClonesArray.h"
#include "TStarTrack.h"
#include "TStarTower.h"

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

void TStarEvent::ClearTrackArray(){
    Tracks->Clear();
}

void TStarEvent::ClearTowerArray(){
    Towers->Clear();
}

void TStarEvent::SetPrimaryVertex(TVector3& pVtx){
    _pVtx_Z = pVtx.z();
    _pVtx_r = pVtx.Perp();
}
#define TStarMixedEvent_cxx

#include "TStarMixedEvent.h"
#include "TStarEvent.h"
#include "TVector3.h"

ClassImp(TStarMixedEvent);

TStarMixedEvent::TStarMixedEvent(){}

TStarMixedEvent::TStarMixedEvent(unsigned int runid, unsigned int eventid){
    _RunID = runid;
    _EventID = eventid;
}

TStarMixedEvent::TStarMixedEvent(const TStarMixedEvent& ev){
    setEvent(ev);
}

TStarMixedEvent::TStarMixedEvent(const TStarEvent& ev){
    setEvent(ev);
}

TStarMixedEvent::~TStarMixedEvent(){}

void TStarMixedEvent::setEvent(const TStarMixedEvent& ev){
    _RunID = ev.runNumber();
    _EventID = ev.eventNumber();
    _gRefMult = ev.gRefMult();
    _RefMult = ev.refMult();
    _RefMultCorr = ev.refMultCorr();
    _Centrality = ev.centrality();
    _RefMultWeight = ev.refmultWeight();
    _MB05to30ReWeight = ev.reWeight_MB05to30();
    _pVtx_Z = ev.Vz();
    _pVtx_r = ev.Vr();
    _VPD_Vz = ev.VPD_Vz();
    _ZDCxx = ev.ZDC_Coincidence();
    _BBCxx = ev.BBC_Coincidence();
}

void TStarMixedEvent::setEvent(const TStarEvent& ev){
    _RunID = ev.runNumber();
    _EventID = ev.eventNumber();
    _gRefMult = ev.gRefMult();
    _RefMult = ev.refMult();
    _RefMultCorr = ev.refMultCorr();
    _Centrality = ev.centrality();
    _RefMultWeight = ev.refmultWeight();
    _pVtx_Z = ev.Vz();
    _pVtx_r = ev.Vr();
    _VPD_Vz = ev.VPD_Vz();
    _ZDCxx = ev.ZDC_Coincidence();
    _BBCxx = ev.BBC_Coincidence();
}

void TStarMixedEvent::setPrimaryVertex(TVector3& p){
    _pVtx_Z = p.Z();
    _pVtx_r = p.Perp();
}

#define TStarGenTrack_cxx

#include "TStarGenTrack.h"
#include "TVector3.h"
#include "TLorentzVector.h"

#include <iostream>

ClassImp(TStarGenTrack);

using namespace std;

TStarGenTrack::TStarGenTrack(){

}

TStarGenTrack::TStarGenTrack(const TStarGenTrack& t) : TStarVector(t){
    _GePid = t._GePid;
    _PDGId = t._PDGId;
    _idVtx_Start = t._idVtx_Start;
    _idVtx_End = t._idVtx_End;
}

TStarGenTrack::~TStarGenTrack(){
    
}

void TStarGenTrack::Print(Option_t *) const {
    cout << "TStarGenTrack: " << endl;
    TStarVector::Print();
    cout <<"Index: " << _Index <<" Charge: " << _Charge << endl;
    cout << "Geant PID: " << _GePid<< " PDG ID: " << _PDGId << endl;
    cout << "Start vertex ID: " << _idVtx_Start << " End vertex ID: " << _idVtx_End << endl;
}


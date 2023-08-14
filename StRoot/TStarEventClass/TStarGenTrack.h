#ifndef TStarGenTrack_h
#define TStarGenTrack_h

#include "TStarVector.h"

class TVector3;
class TLorentzVector;  

class TStarGenTrack : public TStarVector{
public:
    TStarGenTrack();
    TStarGenTrack(const TStarGenTrack& t);
    virtual ~TStarGenTrack();

    int geantId() {return _GePid;}
    int pdgId() {return _PDGId;}
    int idVtxStart() {return _idVtx_Start;}
    int idVtxStop() {return _idVtx_End;}

    void setGeantId(int id) {_GePid = id;}
    void setPdgId(int id) {_PDGId = id;}
    void setIdVtxStart(int id) {_idVtx_Start = id;}
    void setIdVtxEnd(int id) {_idVtx_End = id;}

    virtual void Print(Option_t *option = "") const;

    int _GePid = -99;
    int _PDGId = -99;

    int _idVtx_Start = -99; //!
    int _idVtx_End = -99; //!

    ClassDef(TStarGenTrack, 1)
};


#endif
#ifndef TStarJetEvent_h
#define TStarJetEvent_h

#include "TObject.h"

class TClonesArray;
class TStarJet;
class TStarEvent;

namespace fastjet{
    class PseudoJet;
}

class TStarJetEvent : public TObject{
public:
    TStarJetEvent();
    TStarJetEvent(int runid, int eventid);
    TStarJetEvent(TStarEvent *e);
    virtual ~TStarJetEvent();

    int RunNumber(){return _RunID;}
    int EventNumber(){return _EventID;}
    float Rho(){return _Rho;}
    float Sigma(){return _Sigma;}
    int NumberOfJets(){return _NJets;}
    TClonesArray* GetJets(){return Jets;}

    void SetIdNumbers(int runid, int eventid){_RunID = runid; _EventID = eventid;}
    TStarJet* AddJet(fastjet::PseudoJet& jet);
    void SetRho(float rho){_Rho = rho;}
    void SetSigma(float sig){_Sigma = sig;}
    void SetNumberOfJets(int njets){_NJets = njets;}

    void ClearJetArray();

    int _RunID = 0;
    int _EventID = 0;
    float _Rho = 0;
    float _Sigma = 0;
    float _NJets = 0;
    TClonesArray *Jets = nullptr;

    ClassDef(TStarJetEvent, 1);
};



#endif
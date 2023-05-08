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
    double Rho(){return _Rho;}
    double Sigma(){return _Sigma;}
    int NumberOfJets(){return _NJets;}
    TClonesArray* GetJets(){return Jets;}

    void SetIdNumbers(int runid, int eventid){_RunID = runid; _EventID = eventid;}
    TStarJet* AddJet(fastjet::PseudoJet& jet);
    void SetRho(double rho){_Rho = rho;}
    void SetSigma(double sig){_Sigma = sig;}
    void SetNumberOfJets(int njets){_NJets = njets;}

    void ClearJetArray();

    int _RunID = 0;
    int _EventID = 0;
    double _Rho = 0;
    double _Sigma = 0;
    double _NJets = 0;
    TClonesArray *Jets = nullptr;

    ClassDef(TStarJetEvent, 1);
};



#endif
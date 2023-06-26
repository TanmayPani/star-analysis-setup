#ifndef TStarJetEvent_h
#define TStarJetEvent_h

#include "TObject.h"

class TClonesArray;
class TStarJet;

class TStarJetEvent : public TObject{
public:
    TStarJetEvent();
    TStarJetEvent(float rho, float sigma, int njets);
    TStarJetEvent(const TStarJetEvent& je);
    virtual ~TStarJetEvent();

    float rho(){return _Rho;}
    float sigma(){return _Sigma;}
    int numberOfJets(){return _NJets;}
    TClonesArray* getJets(){return Jets;}

    TStarJet* addJet();
    void addJet(const TStarJet& j);

    void setRho(float rho){_Rho = rho;}
    void setSigma(float sig){_Sigma = sig;}
    void setNumberOfJets(int njets){_NJets = njets;}

    void clearJetArray();

    float _Rho = 0;
    float _Sigma = 0;
    int _NJets = 0;
    TClonesArray *Jets = nullptr;

    ClassDef(TStarJetEvent, 1);
};



#endif
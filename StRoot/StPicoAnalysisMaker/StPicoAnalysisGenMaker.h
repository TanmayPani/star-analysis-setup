#ifndef StPicoAnalysisGenMaker_h
#define StPicoAnalysisGenMaker_h

#include "StPicoAnalysisBaseMaker.h"

//C++ includes
#include <map>
#include <set>
#include <vector>

//ROOT includes
#include "TVector3.h"

class StPicoMcTrack;
class StMyJetMaker;
class StPythiaEvent;
class StPythiaEventMaker;

class StPicoAnalysisGenMaker : public StPicoAnalysisBaseMaker {
public:
    StPicoAnalysisGenMaker(unsigned int imaker, string name, string output, bool dodebug = false);
    virtual ~StPicoAnalysisGenMaker();

    // class required functions
    virtual Int_t Init();
    virtual Int_t Make();
//    virtual void Clear(Option_t *option="");
//    virtual Int_t Finish();

    void setDoPythiaEvent(bool b){doPythiaEvent = b;}

private:
    void runOverGenTracks();
    bool isGenTrackGood(StPicoMcTrack *track);

    virtual void declareStatHistos();
    //virtual void deleteHistograms();

    StPythiaEventMaker *pythiaEventMaker = nullptr;
    StPythiaEvent *pythiaEvent = nullptr;

    //bool doParticleDebug = false;
    bool doPythiaEvent = false;

    double maxParticlePt = 0;

    std::map<unsigned int, unsigned int> genTrackIndexMap;

    ClassDef(StPicoAnalysisGenMaker, 1)
};

#endif
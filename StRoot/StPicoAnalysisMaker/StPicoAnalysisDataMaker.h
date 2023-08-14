#ifndef StPicoAnalysisDataMaker_h
#define StPicoAnalysisDataMaker_h

#include "StPicoAnalysisBaseMaker.h"

//C++ includes
#include <map>
#include <vector>

class StRefMultCorr;
class StEmcPosition2;

class StPicoAnalysisDataMaker : public StPicoAnalysisBaseMaker {
public:
    StPicoAnalysisDataMaker(unsigned int imaker, string name, string output, bool dodebug = false);
    virtual ~StPicoAnalysisDataMaker();

    enum HadronicCorrectionType{kNone = 0, kHighestMatchedTrackE = 1, kFull = 2};

    // class required functions
    virtual Int_t Init();
    virtual Int_t Make();
    //virtual void Clear(Option_t *option="");
    //virtual Int_t Finish();

    void setTowerHadronicCorrType(HadronicCorrectionType t){hadronicCorrType = t;}
    void setDoMixedEventAnalysis(bool b){doMixedEventAnalysis = b;}

private:
    virtual bool isEventGood();

    //To get event related info...
    void runOverTracks();
    void runOverTowers();
    
    virtual void declareStatHistos();
    void initMixedEvents();

    HadronicCorrectionType hadronicCorrType = HadronicCorrectionType::kFull;

    //Points to modified StEmcPosition class instance to 
    //get position vectors of towers given a primary vertex...
    StEmcPosition2 *emcPosition = nullptr;

    //2-D vector containing all tracks matched to tower
    std::vector<double>        towerHadCorrSumTrE ;  
    std::vector<double>        towerHadCorrMaxTrE ;
    std::vector<unsigned int>  towerNTracksMatched;

    //Mixed event analysis variables

    bool doMixedEventAnalysis = false;

    StRefMultCorr *grefmultCorrUtil = nullptr; //for refmult correction
    double MB5toMB30Scale = 1.0;
    bool isMB;

    int mixedEventRefmultBin = -1;
    int mixedEventZVtxBin = -1;
    int mixedEventTrkPtBin = -1;
    int mixedEventBin = -1;

    int nMixTracksFilled = 0;
    int nMixTracksMax = 1000;

    static const unsigned int nRefMultMixBins = 39;
    static const unsigned int nTrkPtMixBins = 6;
    static const unsigned int nZVtxMixBins = 15; 

    ClassDef(StPicoAnalysisDataMaker, 1)
};

#endif
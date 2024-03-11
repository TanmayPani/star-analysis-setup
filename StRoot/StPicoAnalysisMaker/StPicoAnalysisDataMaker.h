#ifndef StPicoAnalysisDataMaker_h
#define StPicoAnalysisDataMaker_h

#include "StPicoAnalysisBaseMaker.h"

//C++ includes
#include <map>
#include <vector>

class StRefMultCorr;
class TStarMixedEvent;

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
    void setTowerHadronicCorrFrac(double f){hadronicCorrFrac = f;}

    void setSelectHTEventsOnly(bool b){selectHTEventsOnly = b;}
    void setDoMixedEventAnalysis(bool b){doMixedEventAnalysis = b;}

    void setRefmultCorrMB5scaleName(std::string name){refmultCorrMB5scName = name;}
    void setRefmultCorrScaleForWeight(std::string name){refmultCorrScaleForWt = name;}

    //static void setTriggerMap();

private:
    virtual bool isEventGood();
    bool isTrigger(unsigned int trig);
    bool isTrigger(std::string trig);
    bool setUpTriggers();

    //To get event related info...
    void runOverTracks();
    void runOverTowers();
    
    virtual void declareStatHistos();
    void initMixedEvents();

    void doHadronicCorrection(double& towerE, unsigned int itow);

    bool selectHTEventsOnly = false;

    HadronicCorrectionType hadronicCorrType = HadronicCorrectionType::kFull;
    double hadronicCorrFrac = 1.0;

    std::vector<double>        towerHadCorrSumTrE ;  
    std::vector<double>        towerHadCorrMaxTrE ;
    std::vector<unsigned int>  towerNTracksMatched;

    std::vector<unsigned int> eventTriggers;
    std::map<std::string, unsigned int> triggerHistMap;
    std::map<unsigned int, unsigned int> triggerIdHistMap;
    //Mixed event analysis variables
    TStarMixedEvent *mixedEvent = nullptr;

    bool doMixedEventAnalysis = false;
    bool isMixedEvent = false;

    std::string refmultCorrMB5scName = "grefmult_P18ih_VpdMB30_AllLumi_MB5sc";
    std::string refmultCorrScaleForWt = "StRoot/StRefMultCorr/macros/weight_grefmult_vpd30_vpd5_Run14_P18ih_set1.txt";
    StRefMultCorr *grefmultCorrMB5sc = nullptr;
    double MB5toMB30Scale = 1.0;
    bool isMB;

    int mixedEventRefmultBin = -1;
    int mixedEventZVtxBin = -1;
    int mixedEventTrkPtBin = -1;
    int mixedEventBin = -1;

    static const unsigned int nRefMultMixBins = 39;
    static const unsigned int nTrkPtMixBins = 6;
    static const unsigned int nZVtxMixBins = 15; 

    std::map<std::string, std::vector<unsigned int>> triggerMap;

    ClassDef(StPicoAnalysisDataMaker, 1)
};

#endif
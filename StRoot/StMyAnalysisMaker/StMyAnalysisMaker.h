#ifndef StMyAnalysisMaker_h
#define StMyAnalysisMaker_h

#include "StMaker.h"

//C++ includes
#include <map>
#include <set>
#include <vector>

//ROOT includes
#include "TVector3.h"

class StRefMultCorr;
class StEmcPosition2;
class StPicoDstMaker;
class StPicoDst;
class StPicoEvent;
class StPicoTrack;
class StPicoBTowHit;
class TStarEvent;
//class TTree;
class TFile;
class TH1F;
class TH2F;

class StMyAnalysisMaker : public StMaker {
public:
    StMyAnalysisMaker(string name, string output, bool dodebug = false);
    virtual ~StMyAnalysisMaker();

    enum HadronicCorrectionType{kNone = 0, kHighestMatchedTrackE = 1, kFull = 2};
    enum RunFlags{kRun12 = 12, kRun14 = 14};
    //enum Triggers{kVPDMB = 100, kVPDMB5 = 105, kVPDMB30 = 130, kHT1 = 201, kHT1xVPDMB30 = 131, kHT2 = 202, kHT2xVPDMB30 = 132, kHT3 = 203};

    // class required functions
    virtual Int_t Init();
    virtual Int_t Make();
    //virtual Int_t Clear();
    virtual Int_t Finish();

    void setRunFlag(RunFlags run){runFlag = run;}
    void setdoppAnalysis(bool b){doppAnalysis = b;}
    void setdoRunbyRun(bool b){doRunbyRun = b;}
    void setZVtxRange(double min, double max){zVtx_Min = min; zVtx_Max = max;}
    void setAbsZVtxMax(double z){absZVtx_Max = z;}

    //void addTriggerToStore(unsigned char t){triggersToStore.push_back(t);}

    void setCentralityRange(int min, int max){centralityMin = min; centralityMax = max; doCentSelection = true;}
    void setExcludeLowEnergyEvents(bool b){excludeLowEnergyEvents = b;}
    void setSelectHTEventsOnly(bool b){doSelectHTEventsOnly = b;}

    void setTrackPtMin(double m)        {trackPtMin = m;}
    void setTrackPtMax(double m)        {trackPtMax = m;}
    void setTrackEtaMin(double m)       {trackEtaMin = m;}
    void setTrackEtaMax(double m)       {trackEtaMax = m;}
    void setTrackDCAMax(double m)       {trackDCAMax = m;}
    void setTrackNHitsFitMin(double m)  {trackNHitsFitMin = m;}
    void setTrackNHitsRatioMin(double m){trackNHitsRatioMin = m;}
    void setMinTrackPtMax(double m)     {minTrackPtMax = m; excludeLowChargedEnergyEvents = true;}

    void setTowerEnergyMin(double m){towerEnergyMin = m;}
    void setTowerEtaMin(double m)   {towerEtaMin = m;}
    void setTowerEtaMax(double m)   {towerEtaMax = m;}
    void setTowerHadronicCorrType(HadronicCorrectionType t){hadronicCorrType = t;}
    void setJetConstituentMinPt(double pt){jetConstituentMinPt = pt;}

    //Output Methods...
    TStarEvent* getEvent(){return tsEvent;}

private:
    double pi0mass = 0.13957;
    // bad and dead tower list arrays
    std::set<int>        badTowers;
    std::set<int>        deadTowers;
    // bad run list 
    std::set<int>        badRuns;

    //Various functions to initialize stuff in Init()...
    void setUpBadRuns();
    void setUpBadTowers();
    void setUpDeadTowers();
    void declareHistograms();
    void writeHistograms();

    //To check event for triggers...
    double getTrackingEfficiency(double pt, double eta, int centbin, double zdcx, TFile *infile);

    //To set centrality related info...
    bool runStRefMultCorr();

    //Utility functions to run over tracks and towers
    //void RunOverEmcTriggers();
    void runOverTracks();
    void runOverTowers();
    //void runOverTowerClusters(); 

    void bookTree();

    bool isTrackGood(StPicoTrack *track);
    bool isTowerGood(unsigned int itow, StPicoBTowHit *tower);

    void printPicoEvent();

    RunFlags runFlag = RunFlags::kRun14;
    HadronicCorrectionType hadronicCorrType = HadronicCorrectionType::kFull;

    //TFile Object that will contain the .root file containing efficiency histograms
    TFile *efficiencyFile = nullptr;


    //Need these to Set StMaker name, and get required objects from *.PicoDst.root files...
    std::string anaName = "";
    std::string outputFileName = "";
    std::string histoFileName = "";
    StPicoDstMaker *picoDstMaker = nullptr;
    StPicoDst *picoDst = nullptr;
    StPicoEvent *picoEvent = nullptr;

    //StRoot/StRefMultCorr objects for centrality calculation...
    StRefMultCorr *grefmultCorr = nullptr;
    StRefMultCorr *grefmultCorrUtil = nullptr;

    //Points to modified StEmcPosition class instance to 
    //get position vectors of towers given a primary vertex...
    StEmcPosition2 *emcPosition = nullptr;

    //Boolean flags to toggle functionalities
    bool doDebug = false;
    bool doppAnalysis = false;
    bool doRunbyRun = false;
    bool doSelectHTEventsOnly = false;
    //bool doSelectForMBEvents = false;
    bool doCentSelection = false;
    bool excludeLowEnergyEvents = false;
    bool excludeLowChargedEnergyEvents = false;
//    bool doJetAnalysis = false;
//    bool doFullJet = false;

    //Event quality cuts
    double zVtx_Min = -40.0;
    double zVtx_Max = 40.0;
    double absZVtx_Max = 40.0;

    //Event analysis cuts
    double centralityMin = 0;
    double centralityMax = 10; 

    //Track quality cuts
    double trackPtMin = 0.2;
    double trackPtMax = 30.0;
    double trackEtaMin = -1.0;
    double trackEtaMax = 1.0;
    double trackDCAMax = 3.0;
    double trackNHitsFitMin = 15;
    double trackNHitsRatioMin = 0.52;
    double minTrackPtMax = 1.0;

    //Tower quality cuts
    double towerEtaMin = -1.0;
    double towerEtaMax = 1.0;
    double towerEnergyMin = 0.2;

    //2-D vector containing all tracks matched to tower
    std::vector<double>        towerHadCorrSumTrE ;  
    std::vector<double>        towerHadCorrMaxTrE ;
    std::vector<unsigned int> towerNTracksMatched;  

    double maxTrackPt = 0;
    double maxTowerEt = 0;

    //Jet Analysis cuts
    double jetConstituentMinPt = 2.0;

    TFile *treeOut = nullptr;
    TFile *histOut = nullptr;

    TTree *tree = nullptr; 

    unsigned int runID = 0;
    unsigned int eventID = 0;

    TVector3 pVtx;

    int centbin9 = -99;
    int centbin16 = -99;
    int centscaled = -99;
    int ref9 = -99;
    int ref16 = -99;
    double Wt = 1.0;

    std::vector<unsigned int> eventTriggers;

    TStarEvent *tsEvent = nullptr;

    std::map<std::string, TH1F*> histos1D;
    std::map<std::string, TH2F*> histos2D;

    ClassDef(StMyAnalysisMaker, 1)
};

#endif
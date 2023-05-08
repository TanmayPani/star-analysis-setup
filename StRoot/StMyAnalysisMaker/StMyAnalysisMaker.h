#ifndef StMyAnalysisMaker_h
#define StMyAnalysisMaker_h

#include "StMaker.h"

//C++ includes
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
class TStarEvent;
//class TTree;
class TFile;
class TH1F;

class StMyAnalysisMaker : public StMaker {
public:
    StMyAnalysisMaker(string name, string output);
    virtual ~StMyAnalysisMaker();

    enum HadronicCorrectionType{kNone = 0, kHighestMatchedTrackE = 1, kFull = 2};
    enum RunFlags{kRun12 = 12, kRun14 = 14};
    enum MBTriggers{kVPDMB, kVPDMB_extra, kVPDMB5, kVPDMB30, kVPDMB30_extra};
    enum HTTriggers{kHT0, kHT1, kHT2, kHT3};

    // class required functions
    virtual Int_t Init();
    virtual Int_t Make();
    //virtual Int_t Clear();
    virtual Int_t Finish();

    void SetRunFlag(RunFlags run){Run_Flag = run;}
    void SetZVtxRange(double min, double max){ZVtx_Min = min; ZVtx_Max = max;}
    void SetAbsZVtxMax(double z){AbsZVtx_Max = z;}
    void SetCentralityRange(double min, double max){CentralityMin = min; CentralityMax = max; doCentSelection = true;}

    void SetdoppAnalysis(bool b){doppAnalysis = b;}
    void SetdoRunbyRun(bool b){doRunbyRun = b;}
    void SetdoHTEventsOnly(bool b){doHTEventsOnly = b;}
    void SetdoMBEventsOnly(bool b){doMBEventsOnly = b;}
    void SetdoCentralitySelection(bool b){doCentSelection = b;}
//    void SetdoJetAnalysis(bool b) {doJetAnalysis = b;}
//    void SetdoFullJets(bool b){doFullJet = b;}

    void SetTrackPtMin(double m){TrackPtMin = m;}
    void SetTrackPtMax(double m){TrackPtMax = m;}
    void SetTrackEtaMin(double m){TrackEtaMin = m;}
    void SetTrackEtaMax(double m){TrackEtaMax = m;}
    void SetTrackDCAMax(double m){TrackDCAMax = m;}
    void SetTrackNHitsFitMin(double m){TrackNHitsFitMin = m;}
    void SetTrackNHitsRatioMin(double m){TrackNHitsRatioMin = m;}

    void SetTowerEnergyMin(double m){TowerEnergyMin = m;}
    void SetTowerEtaMin(double m) {TowerEtaMin = m;}
    void SetTowerEtaMax(double m) {TowerEtaMax = m;}
    void SetTowerHadronicCorrType(HadronicCorrectionType t){TypeOfHadCorr = t;}
    void SetJetConstituentMinPt(bool pt){JetConstituentMinPt = pt;}

    //Output Methods...
    TStarEvent* GetEvent(){return _Event;}

private:
    double pi0mass = 0.13957;
    // bad and dead tower list functions and arrays
    std::set<int>        badTowers;
    std::set<int>        deadTowers;
    // bad run list 
    std::set<int>        badRuns;

    //Various functions to initialize stuff in Init()...
    void SetUpBadRuns();
    void SetUpBadTowers();
    void SetUpDeadTowers();
    void DeclareHistograms();
    void WriteHistograms();

    //To check event for triggers...
    bool IsEventMB(MBTriggers mbtype);
    bool IsEventHT(HTTriggers httype);
    double GetTrackingEfficiency(double pt, double eta, int centbin, double zdcx, TFile *infile);

    //Utility functions to run over tracks and towers
    void RunOverEmcTriggers();
    void RunOverTracks();
    void RunOverTowers();
    void RunOverTowerClusters(); 

    void BookTree();

    RunFlags Run_Flag = RunFlags::kRun14;
    HadronicCorrectionType TypeOfHadCorr = HadronicCorrectionType::kFull;

    //TFile Object that will contain the .root file containing efficiency histograms
    TFile *EfficiencyFile = nullptr;

    //Need these to Set StMaker name, and get required objects from *.PicoDst.root files...
    std::string AnaName = "";
    std::string OutputFileName = "";
    StPicoDstMaker *picoDst_Maker = nullptr;
    StPicoDst *picoDst = nullptr;
    StPicoEvent *picoEvent = nullptr;

    //StRoot/StRefMultCorr objects for centrality calculation...
    StRefMultCorr *grefmultCorr = nullptr;
    StRefMultCorr *grefmultCorrUtil = nullptr;

    //Points to modified StEmcPosition class instance to 
    //get position vectors of towers given a primary vertex...
    StEmcPosition2 *EmcPosition = nullptr;

    //Boolean flags to toggle functionalities
    bool doppAnalysis = false;
    bool doRunbyRun = false;
    bool doHTEventsOnly = false;
    bool doMBEventsOnly = false;
    bool doCentSelection = false;
//    bool doJetAnalysis = false;
//    bool doFullJet = false;

    //Event quality cuts
    float ZVtx_Min = -40.0;
    float ZVtx_Max = 40.0;
    float AbsZVtx_Max = 40.0;

    //Event analysis cuts
    float CentralityMin = 0;
    float CentralityMax = 10; 

    //Track quality cuts
    double TrackPtMin = 0.2;
    double TrackEtaMin = -1.0;
    double TrackEtaMax = 1.0;
    double TrackDCAMax = 3.0;
    double TrackNHitsFitMin = 15;
    double TrackNHitsRatioMin = 0.52;

    //Tower quality cuts
    double TowerEtaMin = -1.0;
    double TowerEtaMax = 1.0;
    double TowerEnergyMin = 0.2;

    //2-D vector containing all tracks matched to tower
    std::vector<std::vector<bool>> HighTowerStatus;
    std::vector<std::vector<int>> TracksMatchedToTower;  

    double TrackPtMax = 0;
    double TowerEtMax = 0;

    //Jet Analysis cuts
    double JetConstituentMinPt = 2.0;

    TFile *fout = nullptr;
    TTree *tree = nullptr; 

protected:
    std::vector<unsigned int> EventTriggers;

    int RunID = 0;
    int EventID = 0;

    int centbin9 = -99;
    int centbin16 = -99;
    int ref9 = -99;
    int ref16 = -99;

    TVector3 pVtx;

    TStarEvent *_Event = nullptr;

    TH1F *hEventStats = nullptr;
    TH1F *hTrackStats = nullptr;
    TH1F *hTowerStats = nullptr;
 
    ClassDef(StMyAnalysisMaker, 2)
};


#endif
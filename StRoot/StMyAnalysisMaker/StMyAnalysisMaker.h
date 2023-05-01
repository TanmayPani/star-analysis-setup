#ifndef StMyAnalysisMaker_h
#define StMyAnalysisMaker_h

#include "StMaker.h"

//C++ includes
#include <set>
#include <vector>
#include <map>

//ROOT includes
#include "TTree.h"
#include "TFile.h"
#include "TVector3.h"
#include "TLorentzVector.h"

using namespace std;

class StRefMultCorr;
class StEmcPosition2;
class StPicoDstMaker;
class StPicoDst;
class StPicoEvent;
class StPicoTrack;
class MyStEvent;
//class TVector3;

class StMyAnalysisMaker : public StMaker {
public:
    StMyAnalysisMaker(string name, string output);
    virtual ~StMyAnalysisMaker();

    enum HadronicCorrectionType{kNone, kHighestMatchedTrackE, kFull};
    enum RunFlags{kRun12, kRun14};
    enum MBTriggers{kVPDMB, kVPDMB_extra, kVPDMB5, kVPDMB30, kVPDMB30_extra};
    enum HTTriggers{kHT0, kHT1, kHT2, kHT3};

    // class required functions
    virtual Int_t Init();
    virtual Int_t Make();
    //virtual Int_t Clear();
    virtual Int_t Finish();

    void SetRunFlag(RunFlags run){Run_Flag = run;}
    void SetZVtxMin(float z){ZVtx_Min = z;}
    void SetZVtxMax(float z){ZVtx_Max = z;}

    void SetdoppAnalysis(bool b){doppAnalysis = b;}
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
    void SetTowerHadronicCorrType(HadronicCorrectionType t){TypeOfHadCorr = t;}
    void SetJetConstituentMinPt(bool pt){JetConstituentMinPt = pt;}

    //Output Methods...
    MyStEvent* GetEvent(){return _Event;}

private:
    double pi0mass = 0.13957;
    // bad and dead tower list functions and arrays
    set<int>        badTowers;
    set<int>        deadTowers;
    // bad run list 
    set<int>        badRuns;

    //May make these SetUp functions public or protected...
    void SetUpBadRuns();
    void SetUpBadTowers();
    void SetUpDeadTowers();

    //To check event for triggers...
    bool IsEventMB(MBTriggers mbtype);
    double GetTrackingEfficiency(TVector3& p, int centbin, double zdcx, TFile *infile);
    //void IsEventHT(HTTriggerTypes httype);

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
    string AnaName = "";
    string OutputFileName = "";
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
//    bool doJetAnalysis = false;
//    bool doFullJet = false;

    //Event quality cuts
    float ZVtx_Min = -40.0;
    float ZVtx_Max = 40.0;

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
    vector<vector<bool>> HighTowerStatus;
    vector<vector<int>> TracksMatchedToTower;  

    double TrackPtMax = 0;
    double TowerEtMax = 0;

    //Jet Analysis cuts
    double JetConstituentMinPt = 2.0;

    TFile *fout = nullptr;
    TTree *tree = nullptr;  

protected:
    vector<unsigned int> EventTriggers;

    int RunID = 0;
    int EventID = 0;

    int centbin9 = -99;
    int centbin16 = -99;
    int ref9 = -99;
    int ref16 = -99;

    TVector3 pVtx;

    MyStEvent *_Event = nullptr;
 
    ClassDef(StMyAnalysisMaker, 2)
};


#endif
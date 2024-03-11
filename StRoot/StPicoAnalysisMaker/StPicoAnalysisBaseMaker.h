#ifndef StPicoAnalysisBaseMaker_h
#define StPicoAnalysisBaseMaker_h

#include "StMaker.h"

//C++ includes
#include <set>
#include <vector>

//ROOT includes
#include "TVector3.h"

class StMyJetMaker;

class StRefMultCorr;

class StPicoDstMaker;
class StPicoDst;
class StPicoEvent;
class StPicoTrack;
class StPicoBTowHit;
class StEmcPosition2;

class TStarArrays;
class TStarEvent;
class TStarJet;

class TFile;
class TH1F;
class TH2F;

class StPicoAnalysisBaseMaker : public StMaker {
public:
    StPicoAnalysisBaseMaker(unsigned int imaker, string name, string output, bool dodebug = false);
    virtual ~StPicoAnalysisBaseMaker();

    // class required functions
    virtual Int_t Init();
    virtual Int_t Finish();

    void setDoEventDebug(bool b){doEventDebug = b;}
    void setDoTrackDebug(bool b){doTrackDebug = b;}
    void setDoTowerDebug(bool b){doTowerDebug = b;}
    void setDoJetDebug(bool b){doJetDebug = b;}
    void setDoTriggerDebug(bool b){doTriggerDebug = b;}

    void setDoppAnalysis(bool b){doppAnalysis = b;}
    void setDoRunbyRun(bool b){doRunbyRun = b;}
    void setDoJetAnalysis(bool b){doJetAnalysis = b;}
    void setDoEmbedding(bool b){doEmbedding = b;}

    void setZVtxRange(double min, double max){zVtx_Min = min; zVtx_Max = max;}
    void setAbsZVtxMax(double z){absZVtx_Max = z; zVtx_Min = -z; zVtx_Max = z;}
    void setCentralityRange(int min, int max){centralityMin = min; centralityMax = max; doCentSelection = true;}
    void setExcludeNoJetEvents(bool b){excludeNoJetEvents = b;}
    void setWeightMultiplier(double w){Wt0 = w;}

    void setTrackPtMin(double m)        {trackPtMin = m;}
    void setTrackPtMax(double m)        {trackPtMax = m;}
    void setTrackEtaMin(double m)       {trackEtaMin = m;}
    void setTrackEtaMax(double m)       {trackEtaMax = m;}
    void setTrackDCAMax(double m)       {trackDCAMax = m;}
    void setTrackNHitsFitMin(double m)  {trackNHitsFitMin = m;}
    void setTrackNHitsRatioMin(double m){trackNHitsRatioMin = m;}

    void setTowerEnergyMin(double m){towerEnergyMin = m;}
    void setTowerEtaMin(double m)   {towerEtaMin = m;}
    void setTowerEtaMax(double m)   {towerEtaMax = m;}

    void setJetConstituentMinPt(double pt){jetConstituentMinPt = pt;}
    void setEfficiencyFileName(std::string name){efficiencyFileName = name;}
    void setBadTowerListName(std::string name){badTowersList = name;}
    void setDeadTowerListName(std::string name){deadTowersList = name;}
    void setBadRunsListName(std::string name){badRunsList = name;}

    void addHist1D(const std::string& key, const std::string& title, const int& nBinsX, const double& xMin, const double& xMax);
    void addHist1D(const std::string& key, const std::string& title, const int& nBinsX, double* xBins);
    void addHist2D(const std::string& key, const std::string& title, const int& nBinsX, const double& minX, const double& maxX, const int& nBinsY, const double& minY, const double& maxY);
    void addHist2D(const std::string& key, const std::string& title, const int& nBinsX, double* xBins, const int& nBinsY, const double& minY, const double& maxY);
    void addHist2D(const std::string& key, const std::string& title, const int& nBinsX, const double& minX, const double& maxX, const int& nBinsY, double* yBins);
    void addHist2D(const std::string& key, const std::string& title, const int& nBinsX, double* xBins, const int& nBinsY, double* yBins);
    void addHist1D(const std::string& key, TH1F* hist){histos1D[key] = hist;}
    void addHist2D(const std::string& key, TH2F* hist){histos2D[key] = hist;}

    TH1F* getHist1D(const std::string& key){if(histos1D.find(key) != histos1D.end()) return histos1D[key]; else return nullptr;}
    TH2F* getHist2D(const std::string& key){if(histos2D.find(key) != histos2D.end()) return histos2D[key]; else return nullptr;}

    void fillHist1D(const std::string& key, const double& x, const double& weight = 1.0);
    void fillHist2D(const std::string& key, const double& x, const double& y, const double& weight = 1.0);

protected:
   //Various functions to initialize stuff in Init()...
    void setUpBadTowers();
    void setUpDeadTowers();
    void setUpBadRuns();

    virtual void declareStatHistos();
    virtual void deleteHistograms();

    void writeHistograms();
    int runStRefMultCorr(); //To set centrality related info...

    double getTrackingEfficiency(double pt, double eta, int centbin, double zdcx, TFile *infile);

    virtual bool isEventGood();
    virtual bool isTrackGood(StPicoTrack *track);
    virtual bool isTowerGood(unsigned int itow, StPicoBTowHit *tower);

    void addEventToTStarArrays();

    //Need these to Set StMaker name, and get required objects from *.PicoDst.root files.
    unsigned int iMaker = 0;
    std::string thisMakerName = "";
    std::string outputFileName = "";
    std::string histoFileName = "";

    TFile *histOut = nullptr;

    //Event level variables to be used in most derived classes...
    StPicoDstMaker *picoDstMaker = nullptr;
    StMyJetMaker *jetMaker = nullptr;
    StPicoEvent *picoEvent = nullptr;
    TStarEvent *tsEvent = nullptr;
    unsigned int runID = 0;
    unsigned int eventID = 0;
    TVector3 pVtx;

    //Event level centrality variables to be used in most derived classes...
    int centbin9 = -99;
    int centbin16 = -99;
    int centscaled = -99;
    int ref9 = -99;
    int ref16 = -99;
    double correctedRefMult = 0;

    //Points to modified StEmcPosition class instance to 
    //get position vectors of towers given a primary vertex...
    StEmcPosition2 *emcPosition = nullptr;
    TVector3 towPos;

    double Wt = 1.0;
    double Wt0 = 1.0;

    //Histograms to be used in most derived classes...
    std::map<std::string, TH1F*> histos1D;
    std::map<std::string, TH2F*> histos2D;

    //TFile Object that will contain the .root file containing efficiency histograms
    std::string efficiencyFileName = "";
    TFile *efficiencyFile = nullptr;

    //StRoot/StRefMultCorr objects for centrality calculation...
    std::string refmultCorrName = "grefmult_P18ih_VpdMB30_AllLumi";
    StRefMultCorr *grefmultCorr = nullptr;

    bool doppAnalysis = false;

    double pi0mass = 0.13957;
    double PtVal = TMath::Pi();

    // bad and dead tower list arrays
    std::string badTowersList = "";
    std::set<int>        badTowers;
    std::string deadTowersList = "";
    std::set<int>        deadTowers;
    // bad run list 
    std::string badRunsList = "";
    std::set<int>        badRuns;

    //Boolean flags to toggle functionalities
    bool doDebug = false;
    bool doEventDebug = false;
    bool doTriggerDebug = false;
    bool doTrackDebug = false;
    bool doTowerDebug = false;
    bool doJetDebug = false;

    bool doRunbyRun = false;
    bool doCentSelection = false;
    bool doJetAnalysis = false;
    bool doEmbedding = false;
    bool excludeNoJetEvents = false;

    //Event quality cuts
    double zVtx_Min = -40.0;
    double zVtx_Max = 40.0;
    double absZVtx_Max = 40.0;

    //Event analysis cuts
    double centralityMin = 0;
    double centralityMax = 80;

    //Very general Track Analysis cuts
    double trackPtMin = 0.2;
    double trackPtMax = 30.0;
    double trackEtaMin = -1.0;
    double trackEtaMax = 1.0;
    //Track quality cuts
    double trackDCAMax = 3.0;
    double trackNHitsFitMin = 15;
    double trackNHitsRatioMin = 0.52;

    //Tower quality cuts
    double towerEtaMin = -1.0;
    double towerEtaMax = 1.0;
    double towerEnergyMin = 0.2;

    //Jet Analysis cuts
    double jetConstituentMinPt = 2.0;

   double maxTrackPt = 0;
   double maxTowerEt = 0;
   
    ClassDef(StPicoAnalysisBaseMaker, 1);
};

#endif
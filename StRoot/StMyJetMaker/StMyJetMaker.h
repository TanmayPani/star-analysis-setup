#ifndef StMyJetMaker_h
#define StMyJetMaker_h

//STAR includes
#include "StMaker.h"

class TStarJet;
class TStarVector;

class TFile;
class TH1F;
class TH2F;

namespace fastjet{
    class PseudoJet;
    class JetDefinition;
    class GhostedAreaSpec;
    class AreaDefinition;
    class ClusterSequence;
    class ClusterSequenceArea;
    class JetMedianBackgroundEstimator;
    namespace contrib{
        class ConstituentSubtractor;
    }
}

class StMyJetMaker : public StMaker{
    public:
        StMyJetMaker(string name, string outputfile, bool debug = false); //default constructor
        virtual ~StMyJetMaker();

        // class required functions
        virtual Int_t Init();
        virtual Int_t Make();
        virtual void  Clear(Option_t *option="");
        virtual Int_t Finish();

        void addConstituentVector(const TStarVector& v);

        void setJetRadius(double r){R = r;}
        void setJetAlgorithm(std::string alg);
        void setRecombScheme(std::string scheme);
        void setGhostMaxRap(double rap){ghostMaxRap = rap; doAreaCalc = true;}
        void setAreaType(std::string type);
        
        void setBkgJetAlgorithm(std::string alg);
        
        unsigned int clusterJets();
        int clusterAndStoreJets();
        void writeHistograms();
        void declareHistograms();

        double getDeltaR(double eta1, double phi1, double eta2, double phi2);
        double getDeltaR(fastjet::PseudoJet& pj1, fastjet::PseudoJet& pj2);

        void setJetConstituentCut(double pt){jetConstituentMinPt = pt;}
        //Kinematic cuts for jets, can add more as needed...
        void setJetPtMin(double pt)     {jetPtMin = pt;}
        void setJetPtCSMin(double pt)   {jetPtCSMin = pt;}
        void setJetPtMax(double pt)     {jetPtMax = pt;}
        void setJetEtaMin(double eta)   {jetEtaMin = eta;}
        void setJetEtaMax(double eta)   {jetEtaMax = eta;}
        void setJetAbsEtaMax(double eta){jetAbsEtaMax = eta;}
        void setDoFullJet(bool b){doFullJet = b;}
        void setDoAreaCalculation(bool b){doAreaCalc = b;}
        void setDoBackgroundEstimation(bool b){doBkgEst = b; 
                                                if(b)doAreaCalc = b;}
        void setDoConstituentSubtraction(bool b){doBkgSub = b;
                                                if(b){doAreaCalc = b; doBkgEst = b;}}
        void setUseSameVectorForBkg(bool b){useSameVectorForBkg = b;}

        double getEventRho(){return eventRho;}
        double getEventSigma(){return eventSigma;}

        unsigned int numberOfJets();
        TStarJet* getJet(unsigned int i);

    private:
        class StPseudoJetUserInfo;
        class StPseudoJetContainer;

        StPseudoJetContainer* jetConstituents = nullptr;
        StPseudoJetContainer* fullEvent = nullptr;

        StPseudoJetContainer* jets = nullptr;

        double eventRho = 0.0; //Rho of the event
        double eventSigma = 0.0; //Sigma of the event

        bool doDebug = false;

        std::string histoFileName = "";

        bool doFullJet = false;
        bool doAreaCalc = false;
        bool doBkgEst = false;
        bool doBkgSub = false;

        bool isGenLevel = false;

        bool useSameVectorForBkg = true;

        float R = 0.4;
        double maxRap = 1.2;
        double jetConstituentMinPt = 2.0;
        double jetPtMin = 10;
        double jetPtCSMin = 8;
        double jetPtMax = 80;
        double jetEtaMin = -0.6;
        double jetEtaMax = 0.6;
        double jetAbsEtaMax = 0.6;

        int jetAlgorithm = 2;
        int recombScheme = 6;
        int areaType = 1;
        double ghostMaxRap = 1.2;

        int bkgJetAlgorithm = 0;

        fastjet::JetDefinition* jetDef = nullptr;
        fastjet::JetDefinition* bkgJetDef = nullptr;

        fastjet::GhostedAreaSpec* areaSpec = nullptr;
        fastjet::AreaDefinition* areaDef = nullptr;

        fastjet::ClusterSequence* clustSeq = nullptr;
        fastjet::ClusterSequenceArea* clustSeqArea = nullptr;

        fastjet::JetMedianBackgroundEstimator* bkgEstimator = nullptr; 
        fastjet::contrib::ConstituentSubtractor* bkgSubtractor = nullptr;

        TFile *histOut = nullptr;

        double Wt = 1.0; //Weight of the event

        std::map<std::string, TH1F*> histos1D;
        std::map<std::string, TH2F*> histos2D;

        static std::map<std::string, int> fJetAlgorithm;
        static std::map<std::string, int> fJetRecombScheme;
        static std::map<std::string, int> fJetAreaType;

    ClassDef(StMyJetMaker, 1)
};
#endif

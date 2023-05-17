#ifndef StMyJetMaker_h
#define StMyJetMaker_h

#include <memory>

//STAR includes
#include "StMaker.h"

class StMyAnalysisMaker;
class TClonesArray;
class TStarEvent;
class TStarJetEvent;
class TFile;

namespace fastjet{
    class PseudoJet;
    class JetDefinition;
    class GhostedAreaSpec;
    class AreaDefinition;
    class ClusterSequence;
    class ClusterSequenceArea;
    class JetMedianBackgroundEstimator;
}

class StMyJetMaker : public StMaker{
    public:
        StMyJetMaker(string name, string analysis, string outputfile); //default constructor
        virtual ~StMyJetMaker();

        // class required functions
        virtual Int_t Init();
        virtual Int_t Make();
        //virtual Int_t Clear();
        virtual Int_t Finish();
 
        void SetJetRadius(double r){R = r;}

        void BookTree();

        //Boolean flags...
        void SetDoMCJets(bool b){doMCJets = b;}

        //Kinematic cuts for jets, can add more as needed...
        void SetJetPtMin(double pt)     {JetPtMin = pt;}
        void SetJetPtMax(double pt)     {JetPtMax = pt;}
        void SetJetEtaMin(double eta)   {JetEtaMin = eta;}
        void SetJetEtaMax(double eta)   {JetEtaMax = eta;}
        void SetJetAbsEtaMax(double eta){JetAbsEtaMax = eta;}

    private:
        double pi0mass = 0.13957;
        StMyAnalysisMaker *anaMaker = nullptr;
        TStarEvent *myEvent = nullptr;
        std::string Analysis = "";
        std::string OutputFileName = "";
        float R = 0.4;
        double MaxRap = 1.2;
        bool doMCJets = false;
        bool doBkgSubtraction = false;

        double JetConstituentMinPt = 2.0;
        double JetPtMin = 10;
        double JetPtMax = 80;
        double JetEtaMin = -0.6;
        double JetEtaMax = 0.6;
        double JetAbsEtaMax = 0.6;

        fastjet::JetDefinition* jet_def = nullptr;
        fastjet::JetDefinition* bkg_jet_def = nullptr;

        fastjet::GhostedAreaSpec* area_spec = nullptr;
        fastjet::AreaDefinition* area_def = nullptr;

        fastjet::ClusterSequence* CS = nullptr;
        fastjet::ClusterSequenceArea* CS_Area = nullptr;

        fastjet::JetMedianBackgroundEstimator* mBGE = nullptr; 

        TFile *fout = nullptr;
        TTree *tree = nullptr; 
        TClonesArray *_Tracks = nullptr;
        TClonesArray *_Towers = nullptr;  

        class StJetUserInfo;

    protected:
        TStarJetEvent *_JetEvent = nullptr; //Final jets of the events are contained here
        
    ClassDef(StMyJetMaker, 1)
};
//TODO : Methods for seamless transition between StPicoTrack and PseudoJet
//Inspiration to be taken from Pythia8Plugins/FastJet3.h

//class PseudoPicoTrack: public StPicoTrack, public PseudoJet::UserInfoBase{
//    public:
//    PseudoPicoTrack(const StPicoTrack trk) : StPicoTrack(trk) {}
//};
#endif

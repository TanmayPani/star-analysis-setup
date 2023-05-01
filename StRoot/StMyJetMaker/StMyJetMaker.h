#ifndef StMyJetMaker_h
#define StMyJetMaker_h

//STAR includes
#include "StMaker.h"

//C++ includes
#include <vector>
#include <map>

class StMyAnalysisMaker;
class MyStEvent;
class StJetEvent;

namespace fastjet{
    class PseudoJet;
};

using namespace std;

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

        // inline void InputForClustering(int i, short ch, float px, float py, float pz, float E);
        void InputForClustering(vector<fastjet::PseudoJet>& p, int i, short ch, double pt, double eta, double phi, double M);
        void InputForClustering(vector<fastjet::PseudoJet>& p, int i, double E, double eta, double phi, double M); 

        //Boolean flags...
        void SetDoBackgroundCalc(bool b){doBackgroundCalc = b;}
        void SetDoBkgSubtraction(bool b){doBkgSubtraction = b;}

        //Kinematic cuts for jets, can add more as needed...
        void SetJetPtMin(double pt)     {JetPtMin = pt;}
        void SetJetPtMax(double pt)     {JetPtMax = pt;}
        void SetJetEtaMin(double eta)   {JetEtaMin = eta;}
        void SetJetEtaMax(double eta)   {JetEtaMax = eta;}
        void SetJetAbsEtaMax(double eta){JetAbsEtaMax = eta;}

        private:
            double pi0mass = 0.13957;
            StMyAnalysisMaker *anaMaker = nullptr;
            MyStEvent *myEvent = nullptr;
            string Analysis = "";
            string OutputFileName = "";
            float R = 0.4;
            double MaxRap = 1.2;
            bool doBackgroundCalc = true;
            bool doBkgSubtraction = false;

            double JetConstituentMinPt = 2.0;

            double JetPtMin = 10;
            double JetPtMax = 80;
            double JetEtaMin = -0.6;
            double JetEtaMax = 0.6;
            double JetAbsEtaMax = 0.6;

        protected:
            StJetEvent *_JetEvent = nullptr; //Final jets of the events are contained here

        private:
            TFile *fout = nullptr;
            TTree *tree = nullptr; 
        
        ClassDef(StMyJetMaker, 1)

};
//TODO : Methods for seamless transition between StPicoTrack and PseudoJet
//Inspiration to be taken from Pythia8Plugins/FastJet3.h

//class PseudoPicoTrack: public StPicoTrack, public PseudoJet::UserInfoBase{
//    public:
//    PseudoPicoTrack(const StPicoTrack trk) : StPicoTrack(trk) {}
//};
#endif
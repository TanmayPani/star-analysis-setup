#ifndef StJetTreeStruct_h
#define StJetTreeStruct_h

#include <iostream>
#include <vector>
#include <map>

#include "TObject.h"
//#include "FJ_includes.h"

//namespace fastjet{
//    class PseudoJet;
//};

using namespace std;

class StJetConstituent : public TObject{
    public:
        StJetConstituent(){}
        StJetConstituent(int i){
            JetIndex = i;
        }
        StJetConstituent(int i, double pt, double eta, double phi, double e, short ch){
            JetIndex = i;
            Pt = pt;
            Eta = eta;
            Phi = phi;
            E = e;
            Charge = ch;
        }
        virtual ~StJetConstituent(){}

        int EventArrayPos = -99;
        int JetIndex = -99;
        double Pt = 0;
        double Eta = -99;
        double Phi = -99;
        double E = 0;
        short Charge = -99;

        ClassDef(StJetConstituent, 1);
};

class StJet : public TObject{
public:
    StJet(){}
    StJet(double pt, double eta, double phi, double e){
       Pt = pt;
       PtCorr = pt;
       Eta = eta;
       Phi = phi;
       E = e; 
    }
    virtual ~StJet(){}

    double Pt = 0;
    double PtCorr = 0;
    double Eta = -99;
    double Phi = -99;
    double E = 0;
    double Area = 0;
    double NEF = -99;
    double Rho = 0;
    double Sigma = 0; 
    int NConstituents = 0;
    ClassDef(StJet, 2);
};

class StJetEvent : public TObject{
public:
    StJetEvent(){}
    StJetEvent(int runid, int eventid){
        RunID = runid;
        EventID = eventid;
    }

    virtual ~StJetEvent(){}

    void ClearVectors(){
        jets.clear();
        constituents.clear();
    }

    void AddJet(StJet& j){
        jets.push_back(j);
    }

    void AddConstituent(StJetConstituent& c){
        constituents.push_back(c); 
    }          
    
    int RunID = 0;
    int EventID = 0;    
    int NJets = 0;
  //  int LeadingJetIndex = -99;
  //  int SubLeadingJetIndex = -99; 

    double EventRho = 0;
    double EventSigma = 0;

    vector<StJet> jets;
    vector<StJetConstituent> constituents;


    ClassDef(StJetEvent, 2);
};

#endif


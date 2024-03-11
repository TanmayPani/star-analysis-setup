#ifndef StPythiaEventMaker_h
#define StPythiaEventMaker_h

#include "StMaker.h"

#include <string>

class TChain;
class TTree;
class TBranch;
class TFile;
class StPythiaEvent;

class StPythiaEventMaker : public StMaker{
public:
    StPythiaEventMaker(string name, string input_files);
    virtual ~StPythiaEventMaker();

    virtual Int_t Init();
    virtual Int_t Make();
    virtual void  Clear(Option_t* option = "");
    virtual Int_t Finish();

    StPythiaEvent* pythiaEvent()    {return mPythiaEvent;}
    TChain* chain()                 {return mChain;}
    TTree* tree()                   {return mTTree;}

    int EventNumber() {return mEventCounter;}

private:
    Int_t makeTChain();

    StPythiaEvent *mPythiaEvent = nullptr;

    std::string mInputFileName = ""; 

    TChain* mChain = nullptr;
    TTree* mTTree = nullptr;
    TBranch* mTBranch = nullptr;

    int mEventCounter = 0;

    ClassDef(StPythiaEventMaker, 1)
};

#endif
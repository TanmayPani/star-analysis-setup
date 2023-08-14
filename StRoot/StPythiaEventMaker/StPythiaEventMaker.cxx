#define StPythiaEventMaker_cxx

#include "TRegexp.h"
#include "TChain.h"
#include "TClonesArray.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjectSet.h"

#include "StPythiaEventMaker.h"
#include "StPythiaEvent.h"
#include "StRoot/StarRoot/TAttr.h"
#include "StRoot/StarRoot/THelixTrack.h"

#include "StRoot/StarRoot/TDirIter.h"
#include "TSystem.h"

ClassImp(StPythiaEventMaker);

using namespace std;

StPythiaEventMaker::StPythiaEventMaker(string name, string input_files) :
StMaker(name.c_str()){
    mInputFileName = input_files;

    StPythiaEvent::Class()->IgnoreTObjectStreamer();

}

StPythiaEventMaker::~StPythiaEventMaker(){
    delete mPythiaEvent;
}

Int_t StPythiaEventMaker::Init(){
    makeTChain();
    return kStOK;
}

Int_t StPythiaEventMaker::Make(){
    if (!mChain) {
        LOG_WARN << " No input files ... ! EXIT" << endm;
        return kStWarn;
    }
   
    int bytes = mChain->GetEntry(mEventCounter++);
    //cout<<"TChain return = "<<bytes<<" for event: "<<mEventCounter<<endl;
    while( bytes <= 0) {
        if( mEventCounter >= mChain->GetEntriesFast() ) {
            return kStEOF;
        }
        LOG_WARN << "Encountered invalid entry or I/O error while reading event "
                << mEventCounter << " from \"" << mChain->GetName() << "\" input tree\n";
        bytes = mChain->GetEntry(mEventCounter++);
            //cout<<"TChain return = "<<bytes<<" for event: "<<mEventCounter<<endl;
    }
   
    //fillEventHeader();
   
    return kStOK; 
}

void StPythiaEventMaker::Clear(char const*){

}

Int_t StPythiaEventMaker::Finish(){
    if(mChain){
        delete mChain;
        mChain = nullptr;
    }

    return kStOK;
}

Int_t StPythiaEventMaker::makeTChain(){
    if(!mChain) mChain = new TChain("PythiaTree");

    string dirFile = mInputFileName;

    if (dirFile.find(".list") != string::npos) {
        ifstream inputStream(dirFile.c_str());

        if (!inputStream) {
          LOG_ERROR << "ERROR: Cannot open list file " << dirFile << endm;
          return kStErr;
        }

        int nFile = 0;
        string file;
        size_t pos;
        
        while ( getline(inputStream, file) ) {
            // NOTE: our external formatters may pass "file NumEvents"
            //       Take only the first part
            //cout << "DEBUG found " <<  file << endl;
            pos = file.find_first_of(" ");
            if (pos!=string::npos ) file.erase(pos,file.length()-pos);
            //cout << "DEBUG found [" <<  file << "]" << endl;
            size_t mupos = file.find(".MuDst.root");
            if (mupos != string::npos){
               file.replace(mupos, 11, ".pythia.root"); 
            }
            if (file.find(".pythia.root") != string::npos) {
                TFile* ftmp = TFile::Open(file.c_str());
                if (ftmp && !ftmp->IsZombie() && ftmp->GetNkeys()) {
                    LOG_INFO << " Read in pythiaEvent file " << file << endm;
                    mChain->Add(file.c_str());
                    ++nFile;
                }
                if (ftmp) ftmp->Close();
            }
        } // while (getline(inputStream, file))

        LOG_INFO << " Total " << nFile << " files have been read in. " << endm;

    }else{ 
        size_t muposdir = dirFile.find(".MuDst.root");
        //size_t picoposdir = dirFile.find("picoDst.root");
        if(muposdir != string::npos){
            cout<<"File is Mu"<<endl;
            dirFile.replace(muposdir, 11, ".pythia.root");
            mChain->Add(dirFile.c_str()); 
            
        }else if(dirFile.find(".pythia.root") != string::npos){
            mChain->Add(dirFile.c_str());
        }else{
            TDirIter Dir(dirFile.c_str());
            Char_t *name = 0;
            Int_t NFiles = 0;
            while ((name = (Char_t *) Dir.NextFile())) {
                TFile* ftmp = TFile::Open(name);
                if (ftmp && !ftmp->IsZombie() && ftmp->GetNkeys()){
                    LOG_INFO << " Read in pythiaEvent file " << name << endm;
                    mChain->Add(name);
                    ++NFiles;
                }
                if (ftmp) ftmp->Close();
            } // while ((name = (Char_t *) Dir.NextFile()))
            if (! NFiles) {
                LOG_WARN << " No good input file to read ... " << endm;
            }
        }
    }

    // Set branch addresses and pointers
    if (mChain) {
        mChain->SetBranchAddress("PythiaBranch", &mPythiaEvent, &mTBranch);
        mTTree = mChain->GetTree();
        mChain->SetCacheSize(50e6);
        mChain->AddBranchToCache("*");
    }

    return kStOK;
}
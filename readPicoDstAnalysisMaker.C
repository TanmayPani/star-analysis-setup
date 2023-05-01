//#pragma link C++ class MyStTrack+;
//#pragma link C++ class MyStTower+;
//#pragma link C++ class MyStEvent+;

#include <TSystem>

// basic STAR classes
class StMemStat;
class StMaker;
class StChain;
class StPicoDstMaker;

// jet-framework STAR classes
class StMyAnalysisMaker;
// constants
const double pi = 1.0*TMath::Pi();

void LoadLibs(){
  // load fastjet libraries 3.x
  //gSystem->Load("libCGAL"); - not installed 
  gSystem->Load("$FASTJET/lib/libfastjet");
  gSystem->Load("$FASTJET/lib/libsiscone");
  gSystem->Load("$FASTJET/lib/libsiscone_spherical");
  gSystem->Load("$FASTJET/lib/libfastjetplugins");
  gSystem->Load("$FASTJET/lib/libfastjettools");
  gSystem->Load("$FASTJET/lib/libfastjetcontribfragile");

  // add include path to use its functionality
  gSystem->AddIncludePath("-I$FASTJET/include");

  // load the system libraries - these were defaults
  gROOT->LoadMacro("$STAR/StRoot/StMuDSTMaker/COMMON/macros/loadSharedLibraries.C");
  loadSharedLibraries();

  // these are needed for new / additional classes
  gSystem->Load("libStPicoEvent");
  gSystem->Load("libStPicoDstMaker");

  // my libraries
  gSystem->Load("StRefMultCorr");
  gSystem->Load("StMyAnalysisMaker");
  gSystem->Load("StMyJetMaker");

  gSystem->ListLibraries();
} 


void readPicoDstAnalysisMaker(string inputFile="testing_Temp.list", string outputFile="test.root", int nEvents = 10000){
      // Load necessary libraries and macros
      LoadLibs();
      // =============================================================================== //
      // open and close output .root file (so it exist and can be updated by Analysis Tasks)

      bool useEmcPidTraits = false;
      bool doEmbedding = false;
      bool makeJetTree = false;

      // create chain
      StChain* chain = new StChain();
      // create the picoMaker maker:  (PicoIoMode, inputFile, name="picoDst")
      // - Write PicoDst's: PicoIoMode::IoWrite -> StPicoDstMaker::IoWrite
      // - Read  PicoDst's: PicoIoMode::IoRead  -> StPicoDstMaker::IoRead
      StPicoDstMaker *picoMaker = new StPicoDstMaker(StPicoDstMaker::IoRead, inputFile.c_str(), "picoDst");
        picoMaker->setVtxMode((int)(StPicoDstMaker::PicoVtxMode::Default));
        picoMaker->SetStatus("*", 0);
        picoMaker->SetStatus("Event", 1);
        picoMaker->SetStatus("Track", 1);
        picoMaker->SetStatus("EmcTrigger", 1);
        picoMaker->SetStatus("BTowHit", 1);
        if(useEmcPidTraits) picoMaker->SetStatus("EmcPidTraits", 1);
      if(doEmbedding){ 
        picoMaker->SetStatus("McVertex", 1);
        picoMaker->SetStatus("McTrack", 1); 
      }
    
      string eventOutputFile = "EventTree_"+outputFile;
      TFile *fout = new TFile(eventOutputFile.c_str(), "RECREATE");
      fout->Close();

      StMyAnalysisMaker *analysis = new StMyAnalysisMaker("StMyAnalysisMaker", eventOutputFile);

      string jetOutputFile = "JetTree_"+outputFile;
      TFile *fjetout = new TFile(jetOutputFile.c_str(), "RECREATE");
      fjetout->Close();

      if(makeJetTree){
        StMyJetMaker *jetMaker = new StMyJetMaker("StMyJetMaker", "StMyAnalysisMaker", jetOutputFile);
      }
       // initialize chain
      chain->Init();
      cout<<"chain->Init();"<<endl;
      int total = picoMaker->chain()->GetEntries();
      cout << " Total entries = " << total << endl;
      if(nEvents > total) nEvents = total;

      for (Int_t i = 0; i < nEvents; i++){
        if(i%100 == 0) cout << "Working on eventNumber " << i << endl;
       // cout << "Working on eventNumber " << i << endl;
        chain->Clear();
        int iret = chain->Make(i);	
        if (iret) { cout << "Bad return code!" << iret << endl; break;}
        total++;		
	    }
	
	cout << "****************************************** " << endl;
	cout << "Work done... now its time to close up shop!"<< endl;
	cout << "****************************************** " << endl;
	chain->Finish();
	cout << "****************************************** " << endl;
	cout << "total number of events  " << nEvents << endl;
	cout << "****************************************** " << endl;
	
	delete chain;	

  // close output file if open
  if(fout->IsOpen())   fout->Close();
  if(fjetout->IsOpen())   fjetout->Close();

  //StMemStat::PrintMem("load StChain");
}

//#pragma link C++ class MyStTrack+;
//#pragma link C++ class MyStTower+;
//#pragma link C++ class MyStEvent+;

#include <TSystem>

// basic STAR classes
class StMemStat;
class StMaker;
class StChain;
class StPicoDstMaker;

// my framework classes
class StMyAnalysisMaker;
class StMyJetMaker;
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
  gSystem->Load("libStRefMultCorr");
  gSystem->Load("libTStarEventClass");
  gSystem->Load("libStMyAnalysisMaker");
  gSystem->Load("libStMyJetMaker");

  gSystem->ListLibraries();
} 

void readPicoDstAnalysisMaker(string inputFile="TESTING_FILELISTS/testing_Temp.list", string outputFile="test.root", int nEvents = 10000){
      // Load necessary libraries and macros
      LoadLibs();
    
      bool useEmcPidTraits = false;
      bool doEmbedding = false;
      bool makeJetTree = false;

      // create chain to take in makers
      StChain* chain = new StChain();
      // create the picoMaker maker:  (PicoIoMode, inputFile, name="picoDst")
      // - Write PicoDst's: PicoIoMode::IoWrite -> StPicoDstMaker::IoWrite
      // - Read  PicoDst's: PicoIoMode::IoRead  -> StPicoDstMaker::IoRead
      StPicoDstMaker *picoMaker = new StPicoDstMaker(StPicoDstMaker::IoRead, inputFile.c_str(), "picoDst");
        picoMaker->setVtxMode((int)(StPicoDstMaker::PicoVtxMode::Default));
        picoMaker->SetStatus("*", 0);
        picoMaker->SetStatus("Event", 1);
        picoMaker->SetStatus("Track", 1);
        //picoMaker->SetStatus("EmcTrigger", 1);
        picoMaker->SetStatus("BTowHit", 1);
        if(useEmcPidTraits) picoMaker->SetStatus("EmcPidTraits", 1);
      if(doEmbedding){ 
        picoMaker->SetStatus("McVertex", 1);
        picoMaker->SetStatus("McTrack", 1); 
      }
    
      string eventOutputFile = "EventTree_"+outputFile;
      TFile *fout = new TFile(eventOutputFile.c_str(), "RECREATE");
      fout->Close();

      string histoOutputFile = "Histograms_"+outputFile;
      TFile *histOut = new TFile(histoOutputFile.c_str(), "RECREATE");
      histOut->Close();


      StMyAnalysisMaker *anaMaker = new StMyAnalysisMaker("StMyAnalysisMaker", outputFile);
      anaMaker->setRunFlag(StMyAnalysisMaker::RunFlags::kRun14);
      anaMaker->setdoppAnalysis(false);
      anaMaker->setdoRunbyRun(true);
      anaMaker->setSelectHTEventsOnly(true);
   
      anaMaker->setJetConstituentMinPt(2.0);
      anaMaker->setAbsZVtxMax(30);
      anaMaker->setExcludeLowEnergyEvents(true);
      anaMaker->setMinTrackPtMax(1.0);
  
      anaMaker->setTrackDCAMax(3.0);
      anaMaker->setTrackNHitsFitMin(15);
      anaMaker->setTrackNHitsRatioMin(0.52);
      anaMaker->setTrackPtMin(0.2);
      anaMaker->setTrackEtaMin(-1.0);
      anaMaker->setTrackEtaMax(1.0);

      anaMaker->setTowerEtaMin(-1.0);
      anaMaker->setTowerEtaMax(1.0);
      anaMaker->setTowerEnergyMin(0.2);
      anaMaker->setTowerHadronicCorrType(StMyAnalysisMaker::HadronicCorrectionType::kFull);

      TFile *fjetout = NULL;
  
      if(makeJetTree){
        string jetOutputFile = "JetTree_"+outputFile;
        fjetout = new TFile(jetOutputFile.c_str(), "RECREATE");
        fjetout->Close();

        StMyJetMaker *jetMaker = new StMyJetMaker("StMyJetMaker", "StMyAnalysisMaker", jetOutputFile);
        jetMaker->SetJetRadius(0.4);
        jetMaker->SetDoMCJets(false);
        jetMaker->SetJetPtMin(10.0);
        jetMaker->SetJetAbsEtaMax(0.6);//1.0-R
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
  if(fout->IsOpen())    fout->Close();
  if(histOut->IsOpen()) histOut->Close();

  if(fjetout != NULL){
    if(fjetout->IsOpen())fjetout->Close();
  }  

  //StMemStat::PrintMem("load StChain");
}

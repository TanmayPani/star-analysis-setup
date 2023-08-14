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
  gSystem->Load("libStPicoAnalysisMaker");
  gSystem->Load("libStPythiaEventMaker");
  gSystem->Load("libStMyJetMaker");

  gSystem->ListLibraries();
} 

void readPicoDstAnalysisMaker(string inputFile="TESTING_FILELISTS/Run14_P18ih_SL20d_mid_testFiles.list", string outputFile="test.root", int nEvents = 100000000){
//void readPicoDstAnalysisMaker(string inputtype="Pythia6Embedding", string trgsetup="AuAu_200_production_2014", string production="P18ih", string library="SL18h", string suffix="20192901_MuToPico20230718", int pthmin = 30, int pthmax = 40, int jobid = 0, string outputFile="test_4.root", int nEvents = 100000000){
      //string inputFile = Form("fileLists/%s_%s_%s_%s_%s/pt%d_%d_%d.list", inputtype.c_str(), trgsetup.c_str(), production.c_str(), library.c_str(), suffix.c_str(), pthmin, pthmax, jobid);
      // Load necessary libraries and macros
      // check if input file is a picoDst file
      if(inputFile.find(".picoDst.root") != std::string::npos){
        cout << "Input file is a picoDst file" << endl;
        outputFile = inputFile.substr(inputFile.find_last_of("/")+1);
        outputFile.replace(outputFile.find(".picoDst.root"), 13, ".root");
      }

      if(inputFile.find(".MuDst.root") != std::string::npos){
        cout << "Input file is a MuDst file" << endl;
        outputFile = inputFile.substr(inputFile.find_last_of("/")+1);
        outputFile.replace(outputFile.find(".MuDst.root"), 13, ".root");
        //cout<<inputFile<<endl;
        //gROOT->ProcessLine(Form(".x genDst.C(-1,\"picoDst,PicoVtxMode:PicoVtxVpdOrDefault,PicoCovMtxMode:PicoCovMtxWrite\",\"%s\"", inputFile.c_str()));
        inputFile.replace(inputFile.find(".MuDst.root"), 11, ".picoDst.root");
        cout<<inputFile<<endl;
      }

      LoadLibs();

      // create chain to take in makers
      StChain* makerChain = new StChain();

      //boolean flags
      bool doppAnalysis = false;
      bool doEmbedding = false;
      bool doMixedEvents = true;
      bool doJetAnalysis = true;

      bool useEmcPidTraits = false;
      bool doFullJets = true;
      bool doPythiaEvent = false;
      bool keepJettyEventsOnly = true;

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

      string histoOutputFile = outputFile;
      histoOutputFile.insert(histoOutputFile.find(".root"), ".hist");
      TFile *histOut = new TFile(histoOutputFile.c_str(), "RECREATE");
      histOut->Close();

      string eventOutputFile = outputFile;
      eventOutputFile.insert(eventOutputFile.find(".root"), ".tree");

      TFile *outFile = new TFile(eventOutputFile.c_str(), "RECREATE");
      //outFile->Close();
      TTree *outTree = new TTree("Events", "Tree with event Info", 99);
      //outTree->SetDirectory(gDirectory);

      string mixedEventOutputFile = outputFile;
      mixedEventOutputFile.insert(mixedEventOutputFile.find(".root"), ".mixed");

      TFile *mixedEventFile = new TFile(mixedEventOutputFile.c_str(), "RECREATE");
      TTree *mixedEventTree = new TTree("MixedEvents", "Tree with mixed events", 99);

      float R = 0.4;
      double jetConstituentPtCut = 2.0;

      TStarArrays *tsArrays = new TStarArrays();
      TStarMixedEventArray *tsMixedEventArray = new TStarMixedEventArray();

      //initialize main analysis arrays...
      TStarArrays::addArray("event");
      TStarArrays::addArray("tracks");
      TStarArrays::addArray("towers");

      int nAnalysisMakers = 0;

      cout<<"adding maker number "<<nAnalysisMakers+1<<" to chain"<<endl;
      StPicoAnalysisDataMaker *dataMaker = new StPicoAnalysisDataMaker(nAnalysisMakers++, "dataMaker", histoOutputFile);
      dataMaker->setRunFlag(14);
      dataMaker->setDoppAnalysis(doppAnalysis);
      dataMaker->setDoRunbyRun(true);
      dataMaker->setSelectHTEventsOnly(true);
      dataMaker->setDoEmbedding(doEmbedding);
      dataMaker->setDoMixedEventAnalysis(doMixedEvents);

      //dataMaker->setDoTriggerDebug(true);
      //dataMaker->setDoJetDebug(true);
      //dataMaker->setDoTrackDebug(true);
      //dataMaker->setDoGenDebug(true);   
      dataMaker->setAbsZVtxMax(30);
  
      dataMaker->setTrackDCAMax(3.0);
      dataMaker->setTrackNHitsFitMin(15);
      dataMaker->setTrackNHitsRatioMin(0.52);
      dataMaker->setTrackPtMin(1.0);
      dataMaker->setTrackEtaMin(-1.0);
      dataMaker->setTrackEtaMax(1.0);

      //dataMaker->setTrackPtMax(30.0);

      dataMaker->setTowerEtaMin(-1.0);
      dataMaker->setTowerEtaMax(1.0);
      dataMaker->setTowerEnergyMin(0.2);
      dataMaker->setTowerHadronicCorrType(StPicoAnalysisDataMaker::HadronicCorrectionType::kFull);

      StMyJetMaker *jetMaker = NULL;
      StMyJetMaker *genJetMaker = NULL;
      StPythiaEventMaker *pyMaker = NULL;

      dataMaker->setDoJetAnalysis(doJetAnalysis);
      dataMaker->setJetConstituentMinPt(jetConstituentPtCut);

      string jetAlgo = "antikt_algorithm";
      string recombScheme = "BIpt2_scheme";

      if(doJetAnalysis){
        TStarArrays::addArray("jets");
        jetMaker = new StMyJetMaker("jetMaker", outputFile);
        jetMaker->setJetAlgorithm(jetAlgo);
        jetMaker->setRecombScheme(recombScheme);
        jetMaker->setJetRadius(R);
        jetMaker->setDoFullJet(doFullJets);
        jetMaker->setJetConstituentCut(jetConstituentPtCut);
        jetMaker->setJetPtMin(5.0);
        jetMaker->setJetPtCSMin(jetConstituentPtCut);
        jetMaker->setJetAbsEtaMax(0.6);//1.0-R
      }

      if(doEmbedding){
        cout<<"adding maker number "<<nAnalysisMakers+1<<" to chain"<<endl;
        StPicoAnalysisGenMaker *genMaker = new StPicoAnalysisGenMaker(nAnalysisMakers++, "genMaker", histoOutputFile);
        genMaker->setTrackPtMin(0.2);
        genMaker->setTrackEtaMin(-1.0);
        genMaker->setTrackEtaMax(1.0); 
        //genMaker->setDoEventDebug(true);
        //genMaker->setDoTrackDebug(true);
        TStarArrays::addArray("genTracks");

        genMaker->setDoPythiaEvent(doPythiaEvent);
        if(doPythiaEvent){
          TStarArrays::addArray("pythiaEvent");
          string pythiaInputFile = inputFile;
          pythiaInputFile.insert(pythiaInputFile.find(".list"), ".pythia");
          pyMaker = new StPythiaEventMaker("pythiaEventMaker", inputFile);
        }

        genMaker->setDoJetAnalysis(doJetAnalysis);
        if(doJetAnalysis){
          genMaker->setJetConstituentMinPt(jetConstituentPtCut); 
          TStarArrays::addArray("genJets");
          genJetMaker = new StMyJetMaker("genJetMaker", outputFile);
          genJetMaker->setJetAlgorithm(jetAlgo);
          genJetMaker->setRecombScheme(recombScheme);
          genJetMaker->setJetRadius(R);
          genJetMaker->setDoFullJet(doFullJets);
          genJetMaker->setJetConstituentCut(jetConstituentPtCut);
          genJetMaker->setJetPtMin(10.0);
          genJetMaker->setJetPtCSMin(8.0);
          genJetMaker->setJetAbsEtaMax(0.6);//1.0-R
        }
      }

      TStarArrays::ignoreTObjectStreamer();

      TStarArrays::setBranch(outTree);
      if(doMixedEvents){
        unsigned int nRefMultMixBins = 39;
        unsigned int nTrkPtMixBins = 6;
        unsigned int nZVtxMixBins = 15; 
        TStarMixedEventArray::addAll(nRefMultMixBins, nZVtxMixBins, nTrkPtMixBins);
        TStarMixedEventArray::ignoreTObjectStreamer();
        TStarMixedEventArray::setBranch(mixedEventTree);
      }
       // initialize chain
      makerChain->Init();
      cout<<"makerChain->Init();"<<endl;
      int total = picoMaker->chain()->GetEntries();
      cout << " Total entries = " << total << endl;
      if(nEvents > total) nEvents = total;

      cout<<"Run Flag = "<<TStarEvent::runFlag()<<endl;

      for (Int_t i = 0; i < nEvents; i++){
        if(i%1000 == 0) cout << "Working on eventNumber " << i << endl;
        //cout << "Working on eventNumber " << i << endl;

        TStarArrays::clearArrays();
       // cout<<TStarArrays::hasEvent()<<" "<<TStarArrays::numberOfTracks()<<endl;
        makerChain->Clear();
        int iret = makerChain->Make(i); 
        if (iret) { cout << "Bad return code!" << iret << endl; break;}
        //cout<<"n reco jets = "<<TStarArrays::numberOfJets()<<" n gen jets = "<<TStarArrays::numberOfGenJets()<<endl;
        if(doJetAnalysis && keepJettyEventsOnly){
          bool hasJets = (TStarArrays::numberOfJets() > 0);
          if(hasJets)dataMaker->fillHist1D("hEventStats", 8);
          if(doEmbedding){
            bool hasGenJets = (TStarArrays::numberOfGenJets() > 0);
            if(hasGenJets)dataMaker->fillHist1D("hEventStats", 9);
            if(!hasJets && !hasGenJets) continue;
          }else if(!hasJets){continue;}
        }
        dataMaker->fillHist1D("hEventStats", 10);
        TStarArrays::setEvent();
        outTree->Fill();	
        total++;		
	    }

  if(doMixedEvents) {
    mixedEventTree->Fill();
    mixedEventFile->WriteObject(mixedEventTree, "MixedEvents");
  }
	cout << "****************************************** " << endl;
	cout << "Work done... now its time to close up shop!"<< endl;
	cout << "****************************************** " << endl;
	makerChain->Finish();
	cout << "****************************************** " << endl;
	cout << "total number of events  " << nEvents << endl;
	cout << "****************************************** " << endl;
	
	delete makerChain;	

  outFile->WriteObject(outTree, "Events");
  // close output file if open
  if(outFile->IsOpen()) outFile->Close();
  if(histOut->IsOpen()) histOut->Close();
  if(mixedEventFile->IsOpen()) mixedEventFile->Close();

  //StMemStat::PrintMem("load StChain");
}

const double pi = 1.0*TMath::Pi();
//Run 12 triggers
unsigned int trg_run12_VPDMB30[2] = {370001, 370011};
unsigned int trg_run12_HT1[2] = {370511, 370546};
unsigned int trg_run12_HT2[4] = {370521, 370522, 370531, 370980};
unsigned int trg_run12_HT3[2] = {380206, 380216};
//Run 14 triggers
unsigned int trg_run14_MBmon[2] = {450011, 450021};
unsigned int trg_run14_VPDMB5[10] = {450005, 450008, 450009, 450014, 450015, 450018, 450024, 450025, 450050, 450060};
unsigned int trg_run14_VPDMB30[2] = {450010, 450020};
unsigned int trg_run14_HT1[2] = {450201, 450211};
unsigned int trg_run14_HT2[2] = {450202, 450212};
unsigned int trg_run14_HT3[2] = {450203, 450213};

//boolean flags
bool doppAnalysis = true;
bool doMixedEvents = false;
bool doJetAnalysis = true;
bool useEmcPidTraits = false;
bool doFullJets = true;
bool doPythiaEvent = false;
bool keepJettyEventsOnly = true;

//event selections
double absZVtxMax = 40.0;
double maxEventPt = 30.0;

//general kinematic selections
double ptMin = 0.2;
double ptMax = 30.0;
double absEtaMax = 1.0;

//track selections
double trackDCAMax = 3.0;
int trackNHitsFitMin = 15;
double trackNHitsRatioMin = 0.52;
double trackPtMin = ptMin;
double trackPtMax = ptMax;
double trackAbsEtaMax = absEtaMax;

//tower selections
double towerAbsEtaMax = absEtaMax;
double towerEnergyMin = ptMin;

//jet definition
float R = 0.4;
double jetConstituentPtCut = 0.2;
string jetAlgo = "antikt_algorithm"; //same format as the enums in fastjet
string recombScheme = "BIpt2_scheme";

//jet selections
double jetPtMin = 5.0;
double jetAbsEtaMax = 1.0-R;

unsigned char runFlag = 12;//This is the only one that matters for now...

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

//void readPicoDstAnalysisMaker(string inputFile="fileLists/testing/pp200GeV_filelist.list", 
void readPicoDstAnalysisMaker(string inputFile="fileLists/testing/st_zerobias_adc_13047003_raw_3600001_r0.picoDst.root", 
//void readPicoDstAnalysisMaker(string inputFile="fileLists/testing/Run14_AuAu200_Pythia6_PicoDst_pt40_50_small.list", 
                              string outputFile="testOutputs/test.root", 
                              int nEvents = 1000000000,
                              bool doEmbedding = true){
    if(runFlag == 12){
      doppAnalysis = true;
      doMixedEvents = false;
    }else if(runFlag == 14){
      doppAnalysis = false;
      doMixedEvents = true;
    }
    // check if input file is a picoDst file
    if((inputFile.find(".picoDst.root") != std::string::npos) && (inputFile.find("*.picoDst.root") == std::string::npos)){
      cout << "Input file is a picoDst file" << endl;
      outputFile = inputFile.substr(inputFile.find_last_of("/")+1);
      outputFile.replace(outputFile.find(".picoDst.root"), 13, ".root");
    }else if(inputFile.find(".MuDst.root") != std::string::npos){
      cout << "Input file is a MuDst file" << endl;
      outputFile = inputFile.substr(inputFile.find_last_of("/")+1);
      outputFile.replace(outputFile.find(".MuDst.root"), 13, ".root");
      //cout<<inputFile<<endl;
      //gROOT->ProcessLine(Form(".x genDst.C(-1,\"picoDst,PicoVtxMode:PicoVtxVpdOrDefault,PicoCovMtxMode:PicoCovMtxWrite\",\"%s\"", inputFile.c_str()));
      inputFile.replace(inputFile.find(".MuDst.root"), 11, ".picoDst.root");
      cout<<inputFile<<endl;
    }

    string efficiencyFileName = "";
    string badTowersList = "";  
    string deadTowersList = "";
    string badRunsList = "";
    switch(runFlag){
      case 12:
        efficiencyFileName = "./additionalFiles/efficiencyHistograms/Run12_efficiency_New.root";
        deadTowersList = "./additionalFiles/towerLists/Y2012_DeadTowers.txt";
        badRunsList = "./additionalFiles/runLists/Y2012_BadRuns_P12id_w_missing_HT.txt";
        if     (jetConstituentPtCut == 0.0)badTowersList = "./additionalFiles/towerLists/Y2012_BadTowers_P12id.txt";
        else if(jetConstituentPtCut == 0.2)badTowersList = "./additionalFiles/towerLists/Y2012_BadTowers_P12id_200MeV.txt";
        else if(jetConstituentPtCut == 1.0)badTowersList = "./additionalFiles/towerLists/Y2012_BadTowers_P12id_1000MeV.txt";
        else if(jetConstituentPtCut == 2.0)badTowersList = "./additionalFiles/towerLists/Y2012_BadTowers_P12id_2000MeV.txt";
        break;
      case 14:
        efficiencyFileName = "./additionalFiles/efficiencyHistograms/Run14_efficiencySmaller2D.root";
        deadTowersList = "./additionalFiles/towerLists/Y2014_DeadTowers_P18ih.txt";
        badRunsList = "./additionalFiles/runLists/Y2014_BadRuns_P18ih_w_missing_HT.txt";
        if     (jetConstituentPtCut == 0.0)badTowersList = "./additionalFiles/towerLists/Y2014_BadTowers_P18ih.txt";
        else if(jetConstituentPtCut == 0.2)badTowersList = "./additionalFiles/towerLists/Y2014_BadTowers_P18ih_200MeV.txt";
        else if(jetConstituentPtCut == 1.0)badTowersList = "./additionalFiles/towerLists/Y2014_BadTowers_P18ih_1000MeV.txt";
        else if(jetConstituentPtCut == 2.0)badTowersList = "./additionalFiles/towerLists/Y2014_BadTowers_P18ih_2000MeV.txt";
        break;
      default://Run14 AuAu for now...
        efficiencyFileName = "./additionalFiles/efficiencyHistograms/Run14_efficiencySmaller2D.root";
        deadTowersList = "./additionalFiles/towerLists/Y2014_DeadTowers_P18ih.txt";
        badRunsList = "./additionalFiles/runLists/Y2014_BadRuns_P18ih_w_missing_HT.txt";
        if     (jetConstituentPtCut == 0.0)badTowersList = "./additionalFiles/towerLists/Y2014_BadTowers_P18ih.txt";
        else if(jetConstituentPtCut == 0.2)badTowersList = "./additionalFiles/towerLists/Y2014_BadTowers_P18ih_200MeV.txt";
        else if(jetConstituentPtCut == 1.0)badTowersList = "./additionalFiles/towerLists/Y2014_BadTowers_P18ih_1000MeV.txt";
        else if(jetConstituentPtCut == 2.0)badTowersList = "./additionalFiles/towerLists/Y2014_BadTowers_P18ih_2000MeV.txt";
        break;
    }
    cout<<"file with efficiency histograms: "<<efficiencyFileName<<endl;  
    cout<<"list file of bad towers: "<<badTowersList<<endl;
    cout<<"list file of dead towers: "<<deadTowersList<<endl;
    cout<<"list file of bad runs: "<<badRunsList<<endl;

//Load necessary libraries and macros
    LoadLibs();
    if(outputFile == "testOutputs/test.root"){
      TDatime date;
      outputFile = Form("testOutputs/test_%d_%d.root", date.GetDate(), date.GetTime());
      nEvents = 10000;
    }
//define an instance of the TStarEvent class to use the static members
    TStarEvent tsEvent;
    TStarEvent::clearTriggerMap();
    if(!doEmbedding){//no need to add triggers for embedding
      switch(runFlag){
      case 12:
        TStarEvent::addTriggerToMap("VPDMB30", trg_run12_VPDMB30, 2);
        TStarEvent::addTriggerToMap("HT1", trg_run12_HT1, 2);
        TStarEvent::addTriggerToMap("HT2", trg_run12_HT2, 4);
        TStarEvent::addTriggerToMap("HT3", trg_run12_HT3, 2);
        break;
      case 14:
        TStarEvent::addTriggerToMap("MBmon", trg_run14_MBmon, 2);
        TStarEvent::addTriggerToMap("VPDMB5", trg_run14_VPDMB5, 10);
        TStarEvent::addTriggerToMap("VPDMB30", trg_run14_VPDMB30, 2);
        TStarEvent::addTriggerToMap("HT1", trg_run14_HT1, 2);
        TStarEvent::addTriggerToMap("HT2", trg_run14_HT2, 2);
        TStarEvent::addTriggerToMap("HT3", trg_run14_HT3, 2);
        break;
      default://Run14 AuAu for now...
        TStarEvent::addTriggerToMap("MBmon", trg_run14_MBmon, 2);
        TStarEvent::addTriggerToMap("VPDMB5", trg_run14_VPDMB5, 10);
        TStarEvent::addTriggerToMap("VPDMB30", trg_run14_VPDMB30, 2);
        TStarEvent::addTriggerToMap("HT1", trg_run14_HT1, 2);
        TStarEvent::addTriggerToMap("HT2", trg_run14_HT2, 2);
        TStarEvent::addTriggerToMap("HT3", trg_run14_HT3, 2);
        break;
      }
    }

    cout<<"Settings: "<<endl;
    cout<<"runFlag: "<<2000+runFlag<<endl;
    cout<<"doppAnalysis: "<<doppAnalysis<<endl;
    cout<<"doMixedEvents: "<<doMixedEvents<<endl;
    cout<<"doJetAnalysis: "<<doJetAnalysis<<endl;
    cout<<"useEmcPidTraits: "<<useEmcPidTraits<<endl;
    cout<<"doFullJets: "<<doFullJets<<endl;
    cout<<"doPythiaEvent: "<<doPythiaEvent<<endl;
    cout<<"keepJettyEventsOnly: "<<keepJettyEventsOnly<<endl;
    cout<<"absZVtxMax: "<<absZVtxMax<<endl;
    cout<<"maxEventPt: "<<maxEventPt<<endl;
    cout<<"ptMin: "<<ptMin<<endl;
    cout<<"ptMax: "<<ptMax<<endl;
    cout<<"absEtaMax: "<<absEtaMax<<endl;
    cout<<"trackDCAMax: "<<trackDCAMax<<endl;
    cout<<"trackNHitsFitMin: "<<trackNHitsFitMin<<endl;
    cout<<"trackNHitsRatioMin: "<<trackNHitsRatioMin<<endl;
    cout<<"trackPtMin: "<<trackPtMin<<endl;
    cout<<"trackPtMax: "<<trackPtMax<<endl;
    cout<<"trackAbsEtaMax: "<<trackAbsEtaMax<<endl;
    cout<<"towerAbsEtaMax: "<<towerAbsEtaMax<<endl;
    cout<<"towerEnergyMin: "<<towerEnergyMin<<endl;
    cout<<"R: "<<R<<endl;
    cout<<"jetConstituentPtCut: "<<jetConstituentPtCut<<endl;
    cout<<"jetAlgo: "<<jetAlgo<<endl;
    cout<<"recombScheme: "<<recombScheme<<endl;
    cout<<"jetPtMin: "<<jetPtMin<<endl;
    cout<<"jetAbsEtaMax: "<<jetAbsEtaMax<<endl;
    //cout<<"efficiencyFileName: "<<efficiencyFileName<<endl;

// create chain to take in StMaker instances
    StChain* makerChain = new StChain();

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
    if(useEmcPidTraits) 
      picoMaker->SetStatus("EmcPidTraits", 1);
    if(doEmbedding){ 
      picoMaker->SetStatus("McVertex", 1);
      picoMaker->SetStatus("McTrack", 1); 
    }

    string histoOutputFile = outputFile;
    histoOutputFile.insert(histoOutputFile.find(".root"), ".hist");
    string eventOutputFile = outputFile;
    eventOutputFile.insert(eventOutputFile.find(".root"), ".tree");

    TFile *histOut = new TFile(histoOutputFile.c_str(), "RECREATE");histOut->Close();
    TFile *outFile = new TFile(eventOutputFile.c_str(), "RECREATE");
    //outFile->Close();
    TTree *outTree = new TTree("Events", "Tree with event Info", 99);
    //outTree->SetDirectory(gDirectory);

    //TStarArrays *tsArrays = new TStarArrays();
    //initialize main analysis arrays...
    TStarArrays::addArray("event");
    TStarArrays::addArray("tracks");
    TStarArrays::addArray("towers");
    if(doEmbedding)
    TStarArrays::addArray("genTracks");
    if(doJetAnalysis)
    TStarArrays::addArray("jets");
    if(doEmbedding && doJetAnalysis)  
    TStarArrays::addArray("genJets");
    if(doEmbedding && doPythiaEvent)
    TStarArrays::addArray("pythiaEvent");

    TFile *mixedEventFile = NULL;
    TTree *mixedEventTree = NULL;
    //TStarMixedEventArray *tsMixedEventArray = NULL;
    if(doMixedEvents){
      string mixedEventOutputFile = outputFile;
      mixedEventOutputFile.insert(mixedEventOutputFile.find(".root"), ".mixed");
      mixedEventFile = new TFile(mixedEventOutputFile.c_str(), "RECREATE");
      mixedEventTree = new TTree("MixedEvents", "Tree with mixed events", 99);
      //tsMixedEventArray = new TStarMixedEventArray();
      TStarMixedEventArray::addArray("event");
      TStarMixedEventArray::addArray("tracks");
    }

    int nAnalysisMakers = 0;
//main detector level analysis maker
    cout<<"adding maker number "<<nAnalysisMakers+1<<" to chain"<<endl;
  StPicoAnalysisDataMaker *dataMaker = new StPicoAnalysisDataMaker(nAnalysisMakers++, "dataMaker", histoOutputFile);
    dataMaker->setDoppAnalysis(doppAnalysis);
    dataMaker->setDoRunbyRun(true);
    dataMaker->setSelectHTEventsOnly(false);
    dataMaker->setDoEmbedding(doEmbedding);
    dataMaker->setDoMixedEventAnalysis(doMixedEvents);
    dataMaker->setEfficiencyFileName(efficiencyFileName);
    dataMaker->setBadTowerListName(badTowersList);
    dataMaker->setDeadTowerListName(deadTowersList);
    dataMaker->setBadRunsListName(badRunsList);
    //dataMaker->setDoTriggerDebug(true);
    //dataMaker->setDoJetDebug(true);
    //dataMaker->setDoTrackDebug(true);
    //dataMaker->setDoGenDebug(true);
//event selections   
    dataMaker->setAbsZVtxMax(absZVtxMax);
    //dataMaker->setTrackPtMax(30.0);
//track selections
    dataMaker->setTrackDCAMax(trackDCAMax);
    dataMaker->setTrackNHitsFitMin(trackNHitsFitMin);
    dataMaker->setTrackNHitsRatioMin(trackNHitsRatioMin);
    dataMaker->setTrackPtMin(trackPtMin);
    dataMaker->setTrackEtaMin(-trackAbsEtaMax);
    dataMaker->setTrackEtaMax(trackAbsEtaMax);
//tower selections
    dataMaker->setTowerEtaMin(-towerAbsEtaMax);
    dataMaker->setTowerEtaMax(towerAbsEtaMax);
    dataMaker->setTowerEnergyMin(towerEnergyMin);
    dataMaker->setTowerHadronicCorrType(StPicoAnalysisDataMaker::HadronicCorrectionType::kFull);
//jet selections
    dataMaker->setDoJetAnalysis(doJetAnalysis);
    dataMaker->setJetConstituentMinPt(jetConstituentPtCut);

    StMyJetMaker *jetMaker = NULL;
    StMyJetMaker *genJetMaker = NULL;
    StPythiaEventMaker *pyMaker = NULL;

    if(doJetAnalysis){
//jet maker instance that will cluster detector level objects (tracks and towers) into jets
    jetMaker = new StMyJetMaker("jetMaker", outputFile);
      jetMaker->setJetAlgorithm(jetAlgo);
      jetMaker->setRecombScheme(recombScheme);
      jetMaker->setJetRadius(R);
      jetMaker->setDoFullJet(doFullJets);
      jetMaker->setJetConstituentCut(jetConstituentPtCut);
      jetMaker->setJetPtMin(jetPtMin);
      jetMaker->setJetPtCSMin(jetConstituentPtCut);
      jetMaker->setJetAbsEtaMax(jetAbsEtaMax);
    }

    if(doEmbedding){
      cout<<"adding maker number "<<nAnalysisMakers+1<<" to chain"<<endl;
//particle level analysis maker
    StPicoAnalysisGenMaker *genMaker = new StPicoAnalysisGenMaker(nAnalysisMakers++, "genMaker", histoOutputFile);
      genMaker->setTrackPtMin(ptMin);
      genMaker->setTrackEtaMin(-absEtaMax);
      genMaker->setTrackEtaMax(absEtaMax); 
      //genMaker->setDoEventDebug(true);
      //genMaker->setDoTrackDebug(true);
      genMaker->setDoPythiaEvent(doPythiaEvent);
      if(doPythiaEvent){//This maker class is not complete yet...
        string pythiaInputFile = inputFile;
        pythiaInputFile.insert(pythiaInputFile.find(".list"), ".pythia");
        pyMaker = new StPythiaEventMaker("pythiaEventMaker", inputFile);
      }

      genMaker->setDoJetAnalysis(doJetAnalysis);
      if(doJetAnalysis){
      genMaker->setJetConstituentMinPt(jetConstituentPtCut); 
//jet maker instance that will cluster particle level objects (particles) into jets
      genJetMaker = new StMyJetMaker("genJetMaker", outputFile);
        genJetMaker->setJetAlgorithm(jetAlgo);
        genJetMaker->setRecombScheme(recombScheme);
        genJetMaker->setJetRadius(R);
        genJetMaker->setDoFullJet(doFullJets);
        genJetMaker->setJetConstituentCut(jetConstituentPtCut);
        genJetMaker->setJetPtMin(jetPtMin);
        genJetMaker->setJetPtCSMin(jetConstituentPtCut);
        genJetMaker->setJetAbsEtaMax(jetAbsEtaMax);
      }
    }

    TStarArrays::ignoreTObjectStreamer();
    TStarArrays::setBranch(outTree);
    if(doMixedEvents){
      TStarMixedEventArray::ignoreTObjectStreamer();
      TStarMixedEventArray::setBranch(mixedEventTree);
    }

    makerChain->Init();//will run the overloaded StMaker::Init() method for each maker in the chain in the order they were added
    cout<<"makerChain->Init();"<<endl;
//The TChain instance set in StPicoDstMaker::Init() will be used to loop over the event in input files
    int total = picoMaker->chain()->GetEntries();
    cout << " Total entries = " << total << endl;
//set the nEvents to a very large number in the argument of readPicoDstAnalysisMaker() to process all events
    if(nEvents > total) nEvents = total;
    cout << " Processing " << nEvents << " events" << endl;
    for (Int_t i = 0; i < nEvents; i++){//event loop
      //if(i%1000 == 0) 
      cout << "Working on eventNumber " << i << endl;
      //Clear all TClonesArray containers in TStarArrays
      TStarArrays::clearArrays();
      if(doMixedEvents) TStarMixedEventArray::clearArrays();
      //run the overloaded StMaker::Make() method for each maker in the chain in the order they were added
      makerChain->Clear(); int iret = makerChain->Make(i); //The TClonesArrays in TStarArrays are filled here... 
      if (iret) { cout << "Bad return code!" << iret << endl; break;}
      //cout<<"number of tracks: "<<TStarArrays::numberOfTracks()<<endl;
      //Fill mixed events tree...
      if(doMixedEvents && TStarMixedEventArray::hasEvent()){TStarMixedEventArray::setEvent(); mixedEventTree->Fill();}
      //Keep only events that have jets
      if(doJetAnalysis && keepJettyEventsOnly){
        bool hasJets = (TStarArrays::numberOfJets() > 0);
        //cout<<"hasJets: "<<hasJets<<endl;
        if(hasJets)dataMaker->fillHist1D("hEventStats", 8);
        if(doEmbedding){
          bool hasGenJets = (TStarArrays::numberOfGenJets() > 0);
          if(hasGenJets)dataMaker->fillHist1D("hEventStats", 9);
          if(!hasJets && !hasGenJets)continue;
        }else if(!hasJets)continue;
      }
      //cout<<"MB5 status: "<<TStarArrays::getEvent()->isMB5()<<endl;
      //cout<<"MB30 status: "<<TStarArrays::getEvent()->isMB30()<<endl;
      //cout<<"HT status: "<<TStarArrays::getEvent()->isHT()<<endl;
      dataMaker->fillHist1D("hEventStats", 10);
      //Fill main analysis tree...
      TStarArrays::setEvent(); outTree->Fill();	
      //total++;		
	  }
	cout << "****************************************** " << endl;
	cout << "Work done... now its time to close up shop!"<< endl;
	cout << "****************************************** " << endl;
	makerChain->Finish();
	cout << "****************************************** " << endl;
	cout << "total number of events  " << nEvents << endl;
	cout << "****************************************** " << endl;
	
	delete makerChain;	

  // write tree to output file
  outFile->WriteObject(outTree, "Events");
  if(doMixedEvents) {
    mixedEventFile->WriteObject(mixedEventTree, "MixedEvents");
    if(mixedEventFile->IsOpen()) mixedEventFile->Close();
  }
  // close output file if open
  if(outFile->IsOpen()) outFile->Close();
  if(histOut->IsOpen()) histOut->Close();
  //StMemStat::PrintMem("load StChain");
}

#define StPicoAnalysisDataMaker_cxx

#include "StPicoAnalysisDataMaker.h"
// ROOT includes
#include "TH1.h"
#include "TH2.h"
// centrality includes
#include "StRoot/StRefMultCorr/CentralityMaker.h"
#include "StRoot/StRefMultCorr/StRefMultCorr.h"
// STAR includes
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoTrack.h"
#include "StPicoEvent/StPicoBTowHit.h"
#include "StPicoEvent/StPicoBEmcPidTraits.h"
//MyAnalysisMaker includes
#include "StEmcPosition2.h"
//#include "StMyAnalysisUtils.h"
#include "StRoot/TStarEventClass/TStarArrays.h"
#include "StRoot/TStarEventClass/TStarEvent.h"
#include "StRoot/TStarEventClass/TStarTrack.h"
#include "StRoot/TStarEventClass/TStarTower.h"
#include "StRoot/TStarEventClass/TStarJet.h"
#include "StRoot/TStarEventClass/TStarMixedEventArray.h"
#include "StRoot/TStarEventClass/TStarMixedEvent.h"
#include "StRoot/TStarEventClass/TStarMixedEventTrack.h"
#include "StRoot/StMyJetMaker/StMyJetMaker.h"

ClassImp(StPicoAnalysisDataMaker);

using namespace std;

//map<string, vector<unsigned int>> StPicoAnalysisDataMaker::triggerMap = {};

StPicoAnalysisDataMaker::StPicoAnalysisDataMaker(unsigned int imaker, string name, string output, bool dodebug):
StPicoAnalysisBaseMaker(imaker, name, output, dodebug){
    cout<<"StPicoAnalysisDataMaker::StPicoAnalysisDataMaker()"<<endl;

    towerHadCorrSumTrE.resize(4800);
    towerHadCorrMaxTrE.resize(4800);
    towerNTracksMatched.resize(4800);

    cout<<"StPicoAnalysisDataMaker::StPicoAnalysisDataMaker() completed"<<endl;
}

StPicoAnalysisDataMaker::~StPicoAnalysisDataMaker(){
    if(doDebug)cout<<"********StPicoAnalysisDataMaker::~StPicoAnalysisDataMaker()*********"<<endl;
    if(emcPosition) delete emcPosition;
    deleteHistograms();
}

Int_t StPicoAnalysisDataMaker::Init(){
    cout<<"***********StPicoAnalysisDataMaker::Init()**************"<<endl;
    triggerMap = TStarEvent::triggerMap();
    
    StPicoAnalysisBaseMaker::Init();

    if(doJetAnalysis){
        if(doJetDebug)cout<<"Getting jet maker..."<<endl;
        jetMaker = static_cast<StMyJetMaker*>(GetMaker("jetMaker"));
        if(!jetMaker){
            cout<<" No jet maker found!"<<endl;
            return kStFatal;
        }if(doJetDebug)cout<<"Got StMyJetMaker!"<<endl;
    }

    setUpBadTowers();//There may be a better way
    setUpDeadTowers();//There may be a better way

    if(doDebug)cout<<"Setting up StEmcPosition2..."<<endl;
    emcPosition = new StEmcPosition2();

    if(doMixedEventAnalysis) {initMixedEvents();}

    if(doDebug)cout<<"***********END OF StPicoAnalysisDataMaker::Init()**************"<<endl;

    return kStOK;
}

void StPicoAnalysisDataMaker::initMixedEvents(){
    grefmultCorrMB5sc = new StRefMultCorr(refmultCorrMB5scName.c_str());
    grefmultCorrMB5sc->setVzForWeight(16, -16.0, 16.0);
    grefmultCorrMB5sc->readScaleForWeight(refmultCorrScaleForWt.c_str());

    //if(doDebug)cout<<"Setting up mixed event arrays..."<<endl;
    //TStarMixedEventArray::addAll(nRefMultMixBins, nZVtxMixBins, nTrkPtMixBins);

    double refMultMixBins[nRefMultMixBins+1] = {0, 28, 57, 83, 106, 127, 146, 164, 181, 197, 212, 226, 240, 253, 266, 278, 290, 302, 314, 326, 337, 348, 359,
                                              370, 381, 392, 403, 414, 425, 436, 447, 458, 469, 480, 492, 504, 517, 532, 551, 700};
    double trkPtMixBins[nTrkPtMixBins+1] = {1.0, 1.5, 2.0, 3.0, 4.0, 6.0, 30.0};

    addHist2D("h2nMix_VzvRefMult", "V_{z} vs RefMult", nZVtxMixBins, zVtx_Min, zVtx_Max, nRefMultMixBins, refMultMixBins);
    addHist2D("h2nMixTracks_VzvRefMult", "V_{z} vs RefMult", nZVtxMixBins, zVtx_Min, zVtx_Max, nRefMultMixBins, refMultMixBins);

    int nMixEvBins = (nZVtxMixBins + 2)*(nRefMultMixBins + 1) - 1;
    addHist2D("h2nMixTracks_EvBinvTrkPt", "Event bin vs track pt", nMixEvBins+1, -0.5, (float)nMixEvBins+0.5, nTrkPtMixBins, trkPtMixBins);
} 

Int_t StPicoAnalysisDataMaker::Make(){
    if(doEventDebug)cout<<"***********StPicoAnalysisDataMaker::Make()**************"<<endl;

    if(!isEventGood()) return kStOK;

    addEventToTStarArrays();

    if(doMixedEventAnalysis){
        //if(makerReturnInt >= 0) return makerReturnInt;
        isMB = (tsEvent->isMB30() || tsEvent->isMB5()) && !tsEvent->isHT();
        if(isMB){
            //cout<<"Adding event to mixed event arrays..."<<endl;
            mixedEventRefmultBin = getHist2D("h2nMix_VzvRefMult")->GetYaxis()->FindBin(correctedRefMult);
            //cout<<"Refmult bin: "<<mixedEventRefmultBin<<endl;
            mixedEventZVtxBin = getHist2D("h2nMix_VzvRefMult")->GetXaxis()->FindBin(pVtx.z());
            //cout<<"Zvtx bin: "<<mixedEventZVtxBin<<endl;
            mixedEventBin = getHist2D("h2nMix_VzvRefMult")->GetBin(mixedEventZVtxBin, mixedEventRefmultBin);
            //cout<<"Mixed event bin: "<<mixedEventBin<<endl;

            grefmultCorrMB5sc->init(runID);
            grefmultCorrMB5sc->initEvent(picoEvent->grefMult(), pVtx.z(), picoEvent->ZDCx()); 
            MB5toMB30Scale = (tsEvent->isMB5() && !tsEvent->isMB30()) ? grefmultCorrMB5sc->getWeight() : 1.0;

            mixedEvent = TStarMixedEventArray::addEvent();
            mixedEvent->setEvent(*(TStarArrays::getEvent()));
            mixedEvent->setReWeight_MB05to30(MB5toMB30Scale);
            mixedEvent->setMB05(tsEvent->isMB5());
            mixedEvent->setMB30(tsEvent->isMB30());

            fillHist2D("h2nMix_VzvRefMult", pVtx.z(), correctedRefMult);
        }
    }   
    

    runOverTracks(); //Runs over all tracks
    runOverTowers(); //Runs over all towers

    if(doEventDebug)cout<<"Max track pt: "<<maxTrackPt<<" Max tower Et: "<<maxTowerEt<<endl;

    fillHist2D("h2MaxTrkPtvTowEt", maxTrackPt, maxTowerEt, Wt);

    if(doEventDebug){
        cout<<"TStarEvent summary: "<<endl;
        tsEvent->Print();
        cout<<"Filling tree"<<endl;
    }

    if(doEventDebug)cout<<"**************Finished StPicoAnalysisDataMaker::Make()********************"<<endl;

    return kStOk;
}

bool StPicoAnalysisDataMaker::isEventGood(){
    if(!StPicoAnalysisBaseMaker::isEventGood()) return false;

    if(!doEmbedding){
        if(!setUpTriggers()) return false;
    }else if(doEventDebug)cout<<"Embedding or mixed-event mode, skipping trigger selection here..."<<endl;

    return true;
}

void StPicoAnalysisDataMaker::runOverTracks(){
    if(doTrackDebug)cout<<"**********StPicoAnalysisDataMaker::runOverTracks()************"<<endl;

    if(doTrackDebug)cout<<"Resetting hadronic correction vectors... "<<endl;
    towerHadCorrSumTrE.assign(4800, 0);
    towerHadCorrMaxTrE.assign(4800, 0);
    towerNTracksMatched.assign(4800, 0);

    if(maxTrackPt > 0){
        if(doTrackDebug)cout<<"Max track pt was at "<<maxTrackPt<<", now set to 0 ..."<<endl;
        maxTrackPt = 0;
    }

    if(TStarArrays::numberOfTracks() > 0){
        cout<<"Track array not cleared from previous event!"<<endl;
    }

    unsigned int nTracks = StPicoDst::numberOfTracks();

    if(doTrackDebug){
        cout<<"*************** Tracks Summary: ***************"<<endl;
        cout<<"Number of tracks: "<<nTracks<<endl;
    }

    for(unsigned int itrk = 0; itrk < nTracks; itrk++){ //begin Track Loop...
        StPicoTrack *trk = StPicoDst::track(itrk);

        if(!isTrackGood(trk)) continue;

        TVector3 trkMom = trk->pMom();
        double trkPt = trkMom.Perp();
        double trkEta = trkMom.Eta();
        double trkPhi = trkMom.Phi();
        short trkChrg = trk->charge();

        int towerMatched = trk->bemcTowerIndex();
        if(towerMatched >= 0){
            double E = sqrt(trkMom.Mag2() + pi0mass*pi0mass);
            towerNTracksMatched[towerMatched]++;   
            towerHadCorrSumTrE[towerMatched] += E;
            towerHadCorrMaxTrE[towerMatched] = max(E, towerHadCorrMaxTrE[towerMatched]);
        }else{fillHist1D("hTrackStats", 7);}

        maxTrackPt = max(trkPt, maxTrackPt);

        TStarTrack* tsTrk = TStarArrays::addTrack();
        tsTrk->setIndex(TStarArrays::numberOfTracks()-1);
        tsTrk->setCharge(trkChrg);
        tsTrk->setVector(trkMom, sqrt(trkMom.Mag2() + pi0mass*pi0mass));
        double trackingEff = getTrackingEfficiency(trkPt, trkEta, ref16, tsEvent->ZDC_Coincidence(), efficiencyFile);
        tsTrk->setTrackingEfficiency(trackingEff);
        tsTrk->setMatchedTower(towerMatched);
        tsTrk->setNSigmas(trk->nSigmaPion(), trk->nSigmaKaon(), trk->nSigmaProton(), trk->nSigmaElectron());

        if(doTrackDebug){
            cout<<"_______Done with track: "<<itrk<<"________"<<endl;
            cout<<"Made StPicoTrack into TStarTrack! comparing"<<endl;
            tsTrk->Print();
            cout<<"Adding to track array..."<<endl;
        }

        if(doJetAnalysis){
            jetMaker->addConstituentVector(*tsTrk);
            if((trkPt > jetConstituentMinPt) && doJetDebug){
                cout<<"StPicoTrack: "<<itrk<<" pt: "<<trkPt<<" eta: "<<trkEta<<" phi: "<<trkPhi<<" charge: "<<trkChrg<<endl;
                cout<<"TStarTrack: "<<tsTrk->index()<<" pt: "<<tsTrk->pt()<<" eta: "<<tsTrk->eta()<<" phi: "<<tsTrk->phi()<<" charge: "<<tsTrk->charge()<<endl;
            }
        }

        if(isMB && doMixedEventAnalysis){
            fillHist2D("h2nMixTracks_VzvRefMult", pVtx.z(), correctedRefMult);
            fillHist2D("h2nMixTracks_EvBinvTrkPt", mixedEventBin, trkPt);
            //cout<<"Adding track to mixed event array..."<<endl;
            TStarMixedEventTrack* mixTrk = TStarMixedEventArray::addTrack();
            mixTrk->setPtEtaPhi(trkPt, trkEta, trkPhi);
            mixTrk->setCharge(trkChrg);
            mixTrk->setTrackingEfficiency(trackingEff);
        }

        fillHist1D("hTrackPt", trkPt, Wt);
        fillHist1D("hTrackPtxCh", trkPt*trkChrg, Wt);
        fillHist1D("hTrackEta", trkEta, Wt); 
        fillHist1D("hTrackPhi", tsTrk->phi(), Wt);

       // fillHist2D("h2TrackPtvEff", trkPt, trackingEff, Wt);
        fillHist2D("h2TrackEtavPhi", tsTrk->phi(), trkEta, trkPt*Wt); 
    } //end Track Loop...
    fillHist2D("h2CentvMaxTrackPt", centscaled, maxTrackPt, Wt);
    if(doTrackDebug)cout<<"Final Max track pt: "<<maxTrackPt<<endl;
    tsEvent->setMaxTrackPt(maxTrackPt);
    if(doTrackDebug)cout<<"********** END StPicoAnalysisDataMaker::runOverTracks() done************"<<endl;
}

void StPicoAnalysisDataMaker::runOverTowers(){    
    if(doTowerDebug)cout<<"**********StPicoAnalysisDataMaker::runOverTowers()************"<<endl;
    if(maxTowerEt > 0){
        if(doTowerDebug)cout<<"Max tower Et was at "<<maxTowerEt<<", now set to 0..."<<endl;
        maxTowerEt = 0;
    }
    if(TStarArrays::numberOfTowers() > 0){
        cout<<"Tower array not cleared from previous event!"<<endl;
    }
    unsigned int nTowers = StPicoDst::numberOfBTowHits();
    if(doTowerDebug){
        cout<<"*************** Towers Summary: ***************"<<endl;
        cout<<"Number of towers: "<<nTowers<<endl;
    }
    for(unsigned int itow = 0; itow < nTowers; itow++){
        StPicoBTowHit *tower = StPicoDst::btowHit(itow);

        if(!isTowerGood(itow, tower)) continue;
        double towERaw = tower->energy();

        //Start hardonic correction of tower...
        double towE = towERaw;
        doHadronicCorrection(towE, itow);

        double towEt = towE/cosh(towPos.Eta());
        if(towEt < towerEnergyMin){
            if(doTowerDebug) cout<<"Tower Et: "<<towEt<<" less than minimum: "<<towerEnergyMin<<endl;
            continue;
        }else if(doTowerDebug){
            if(towEt > maxTowerEt)cout<<"Max tower Et changed from: "<<maxTowerEt<<" to "<<towEt<<endl;
        }fillHist1D("hTowerStats", 5);

        maxTowerEt = max(towEt, maxTowerEt);

        fillHist1D("hTowerStats", 6);

        TStarTower* tsTow = TStarArrays::addTower();
        tsTow->setIndex(TStarArrays::numberOfTowers()-1);
        tsTow->setADC(tower->adc());
        tsTow->setRawE(towERaw);
        tsTow->setTowerVector(towPos, towE, pi0mass);
        tsTow->setNMatchedTracks(towerNTracksMatched[itow]);

        if(doTowerDebug){
            cout<<"Made StPicoBTowHit into TStarTower! comparing"<<endl;
            tsTow->Print();
            cout<<"Adding to tower array..."<<endl;
        }
        if(doJetAnalysis){
            jetMaker->addConstituentVector(*tsTow);
            if((towEt > jetConstituentMinPt) && doJetDebug){
                cout<<"StPicoTower: "<<itow<<" Et: "<<towEt<<" eta: "<<towPos.Eta()<<" phi: "<<towPos.Phi()<<endl;
                cout<<"TStarTower:"<<tsTow->index()<<" Pt: "<<tsTow->pt()<<" eta: "<<tsTow->eta()<<" phi: "<<tsTow->phi()<<endl;
            }
        }
        fillHist1D("hTowerERaw", towERaw, Wt);
        fillHist1D("hTowerE", towE, Wt);
        fillHist1D("hTowerEt", towEt, Wt);
        fillHist1D("hTowerEta", towPos.Eta(), Wt);
        fillHist1D("hTowerPhi", tsTow->phi(), Wt);
        fillHist2D("h2TowerdECorr", towerHadCorrSumTrE[itow], towerHadCorrMaxTrE[itow], Wt);
        fillHist2D("h2TowerEtavPhi", tsTow->phi(), towPos.Eta(), towEt*Wt);
    }//end tower loop...
    fillHist2D("h2CentvMaxTowerEt", centscaled, maxTowerEt, Wt);
    if(doTowerDebug)cout<<"Final Max tower Et: "<<maxTowerEt<<endl;
    tsEvent->setMaxTowerEt(maxTowerEt);
    if(doTowerDebug)cout<<"********** END StPicoAnalysisDataMaker::runOverTowers() done************"<<endl;
}

void StPicoAnalysisDataMaker::doHadronicCorrection(double& towE, unsigned int itow){
    if(towerNTracksMatched[itow] != 0){
    if(doTowerDebug){
        cout<<"Tower: "<<itow<<" has "<<towerNTracksMatched[itow]<<" tracks matched ";
        cout<<"sum of energies"<<towerHadCorrSumTrE[itow]<<" max energy: "<<towerHadCorrMaxTrE[itow]<<endl;
    }
    if(hadronicCorrType == HadronicCorrectionType::kFull){towE -= hadronicCorrFrac*towerHadCorrSumTrE[itow];}
    else if(hadronicCorrType == HadronicCorrectionType::kHighestMatchedTrackE){towE -= hadronicCorrFrac*towerHadCorrMaxTrE[itow];}
    if(doTowerDebug)cout<<"Tower energy after hadronic correction: "<<towE<<endl;
    }else{fillHist1D("hTowerStats", 7);
        if(doTowerDebug)cout<<"Tower: "<<itow<<" has no matched tracks"<<endl;
    }

}

bool StPicoAnalysisDataMaker::isTrigger(unsigned int trig){
    return (find(eventTriggers.begin(), eventTriggers.end(), trig) != eventTriggers.end());
}

bool StPicoAnalysisDataMaker::isTrigger(string trig){
    if(doTriggerDebug)cout<<"Checking trigger: "<<trig<<endl;

    if(triggerMap.find(trig) == triggerMap.end()) return false;

    for(auto t : triggerMap[trig]){if(isTrigger(t)) return true;}
    return false;
}

bool StPicoAnalysisDataMaker::setUpTriggers(){
    eventTriggers = picoEvent->triggerIds();
    if(eventTriggers.empty()){
        if(doTriggerDebug)cout<<"No triggers found!"<<endl;
        return false;
    }
    if(doTriggerDebug){
        cout<<"***************************"<<endl;
        cout<<"Reading event Triggers: "<<endl;
        for(const auto& trig : eventTriggers){
            cout<<trig<<endl;
        }
    }
    fillHist1D("hTriggerIdStats", 0);
    fillHist1D("hTriggerStats", 0);
    bool hasTrigger = false;
    for(auto trg : triggerMap){
        bool isTrig = false;
        for(auto id : trg.second){
            if(!isTrigger(id)) continue;
            fillHist1D("hTriggerIdStats", triggerIdHistMap[id]-1);
            isTrig = true;
            if(doTriggerDebug)cout<<triggerIdHistMap[id]-1<<" "<<id<<" event!"<<endl;
        }
        if(!isTrig) continue;
        hasTrigger = true;
        fillHist1D("hTriggerStats", triggerHistMap[trg.first]-1);
        if(doTriggerDebug)cout<<triggerHistMap[trg.first]-1<<" "<<trg.first<<" event!"<<endl;
    }

    if(!hasTrigger){
        if(doTriggerDebug)cout<<"No trigger found!"<<endl;
        return false;
    }

    if(selectHTEventsOnly && !doMixedEventAnalysis){
        bool isHT = isTrigger("HT1") || isTrigger("HT2") || isTrigger("HT3");
        if(!isHT){
            if(doEventDebug || doTriggerDebug)cout<<"Not a HT event!"<<endl;
            return false;
        }
    }

    fillHist1D("hEventStats", 3);
    if(doTriggerDebug)cout<<"**********************"<<endl;

    return true;
}

void StPicoAnalysisDataMaker::declareStatHistos(){
    if(iMaker == 0) StPicoAnalysisBaseMaker::declareStatHistos();
    addHist1D("hTriggerStats", "Trigger Statistics", 10, -0.5, 9.5);
    addHist1D("hTriggerIdStats", "Trigger ID Statistics", 20, -0.5, 19.5);
    addHist1D("hTrackStats", "Track Statistics", 10, -0.5, 9.5);
    addHist1D("hTowerStats", "Tower Statistics", 10, -0.5, 9.5); 

    getHist1D("hTrackStats")->GetXaxis()->SetBinLabel(1, "ALL");
    getHist1D("hTrackStats")->GetXaxis()->SetBinLabel(2, "DCA PASS");
    getHist1D("hTrackStats")->GetXaxis()->SetBinLabel(3, "nHitsFit PASS");
    getHist1D("hTrackStats")->GetXaxis()->SetBinLabel(4, "nHitsRatio PASS");
    getHist1D("hTrackStats")->GetXaxis()->SetBinLabel(5, "PT PASS");
    getHist1D("hTrackStats")->GetXaxis()->SetBinLabel(6, "ETA PASS");
    getHist1D("hTrackStats")->GetXaxis()->SetBinLabel(7, "GOOD");
    getHist1D("hTrackStats")->GetXaxis()->SetBinLabel(8, "TOWER MATCHED");

    getHist1D("hTowerStats")->GetXaxis()->SetBinLabel(1, "ALL");
    getHist1D("hTowerStats")->GetXaxis()->SetBinLabel(2, "GOOD");
    getHist1D("hTowerStats")->GetXaxis()->SetBinLabel(3, "ALIVE");
    getHist1D("hTowerStats")->GetXaxis()->SetBinLabel(4, "RawE PASS");
    getHist1D("hTowerStats")->GetXaxis()->SetBinLabel(5, "ETA PASS");
    getHist1D("hTowerStats")->GetXaxis()->SetBinLabel(6, "Et PASS");
    getHist1D("hTowerStats")->GetXaxis()->SetBinLabel(7, "ALL GOOD");
    getHist1D("hTowerStats")->GetXaxis()->SetBinLabel(8, "NO TRACKS MATCHED");

    int bin = 1;
    int idBin = 1;
    getHist1D("hTriggerStats")->GetXaxis()->SetBinLabel(bin, "ALL");
    getHist1D("hTriggerIdStats")->GetXaxis()->SetBinLabel(idBin, "ALL");
    for(auto trg : triggerMap){
        getHist1D("hTriggerStats")->GetXaxis()->SetBinLabel(++bin, trg.first.c_str());
        triggerHistMap[trg.first] = bin;
        if(doTriggerDebug)cout<<"Setting trigger label: "<<trg.first<<" "<<bin<<endl;
        for(auto id : trg.second){
            getHist1D("hTriggerIdStats")->GetXaxis()->SetBinLabel(++idBin, to_string(id).c_str());
            triggerIdHistMap[id] = idBin;
            if(doTriggerDebug)cout<<"Setting trigger id label: "<<id<<" "<<idBin<<endl;
        }
    }
}
#define StPicoAnalysisGenMaker_cxx

#include "StPicoAnalysisGenMaker.h"
#include "StRoot/StMyJetMaker/StMyJetMaker.h"

// centrality includes
#include "StRoot/StRefMultCorr/CentralityMaker.h"
#include "StRoot/StRefMultCorr/StRefMultCorr.h"
// STAR includes
#include "StPicoDstMaker/StPicoDstMaker.h"
#include "StPicoEvent/StPicoDst.h"
#include "StPicoEvent/StPicoEvent.h"
#include "StPicoEvent/StPicoMcTrack.h"
// TStar includes
#include "StRoot/TStarEventClass/TStarEvent.h"
#include "StRoot/TStarEventClass/TStarTrack.h"
#include "StRoot/TStarEventClass/TStarGenTrack.h"
#include "StRoot/TStarEventClass/TStarTower.h"
#include "StRoot/TStarEventClass/TStarJet.h"
#include "StRoot/TStarEventClass/TStarArrays.h"
// StPythiaMaker includes
#include "StRoot/StPythiaEventMaker/StPythiaEvent.h"
#include "StRoot/StPythiaEventMaker/StPythiaEventMaker.h"

ClassImp(StPicoAnalysisGenMaker);

using namespace std;

StPicoAnalysisGenMaker::StPicoAnalysisGenMaker(unsigned int imaker, string name, string output, bool dodebug):
StPicoAnalysisBaseMaker(imaker, name, output, dodebug){        
    cout<<"StPicoAnalysisGenMaker::StPicoAnalysisGenMaker()"<<endl;
    cout<<"StPicoAnalysisGenMaker::StPicoAnalysisGenMaker() completed"<<endl;
}

StPicoAnalysisGenMaker::~StPicoAnalysisGenMaker(){
    cout<<"********StPicoAnalysisGenMaker::~StPicoAnalysisGenMaker()*********"<<endl;
    deleteHistograms();
}

Int_t StPicoAnalysisGenMaker::Init(){
    cout<<"***********StPicoAnalysisGenMaker::Init()**************"<<endl;
    StPicoAnalysisBaseMaker::Init();

    //declareStatHistos();

    if(doJetAnalysis){
        cout<<"Getting gen-level jet maker..."<<endl;
        jetMaker = static_cast<StMyJetMaker*>(GetMaker("genJetMaker"));
        if(!jetMaker){
            cout<<" No Gen-level jet maker found!"<<endl;
            return kStFatal;
        }cout<<"Got Gen level StJetMaker!"<<endl;
    }

    if(doPythiaEvent){
        pythiaEventMaker = static_cast<StPythiaEventMaker*>(GetMaker("pythiaEventMaker"));
        if(!pythiaEventMaker){
            cout<<" No Pythia event maker found!"<<endl;
            return kStFatal;
        }if(doEventDebug)cout<<"Got Pythia event maker!"<<endl;
    } 

    if(doDebug)cout<<"***********END OF StPicoAnalysisGenMaker::Init()**************"<<endl;

    return kStOK;
}

Int_t StPicoAnalysisGenMaker::Make(){
    if(doEventDebug)cout<<"***********StPicoAnalysisGenMaker::Make()**************"<<endl;

    if(iMaker == 0){
        if(!isEventGood()) return kStOK;
        addEventToTStarArrays();
    }

    if(!TStarArrays::hasEvent()){
        //cout<<"No event found in TStarArrays! SKIP!"<<endl;
        return kStOk;
    }

    if(pythiaEventMaker){
        if(doEventDebug)cout<<"Getting Pythia event..."<<endl;
        pythiaEvent = static_cast<StPythiaEvent*>(pythiaEventMaker->pythiaEvent());
        if(!pythiaEvent){
            cout<<" No Pythia event found! SKIP!"<<endl;
            return kStWarn;
        }if(doEventDebug)cout<<"Got Pythia event!"<<endl;
        if((pythiaEvent->runId() != picoEvent->runId()) || (pythiaEvent->eventId() != picoEvent->eventId())){
            cout<<"Pythia event and Pico event out of sync! SKIP!"<<endl;
            return kStWarn;
        }if(doEventDebug)cout<<"Pythia event and Pico event in sync!"<<endl;
    }

    runOverGenTracks(); //Runs over all gen tracks

    if(doPythiaEvent){
        StPythiaEvent *pyEvt = TStarArrays::addPythiaEvent();
        pyEvt->set(*pythiaEvent);
    } 

    if(doEventDebug)cout<<"**************Finished StPicoAnalysisGenMaker::Make()********************"<<endl;

    return kStOk;
}

void StPicoAnalysisGenMaker::runOverGenTracks(){
    if(doTrackDebug)cout<<"**********StPicoAnalysisGenMaker::runOverGenTracks()************"<<endl;

    if(maxParticlePt > 0){
        if(doTrackDebug)cout<<"Max gen track pt was at "<<maxParticlePt<<", now set to 0 ..."<<endl;
        maxParticlePt = 0;
    }
    if(TStarArrays::numberOfGenTracks() > 0){
        cout<<"Gen Track array not cleared from previous event!"<<endl;
    }
    unsigned int nGenTracks = StPicoDst::numberOfMcTracks();
    if(doTrackDebug){
        cout<<"*************** Gen level Summary: ***************"<<endl;
        cout<<"Number of gen tracks: "<<nGenTracks<<endl;
    }

    for(unsigned int igen = 0; igen < nGenTracks; igen++){
        StPicoMcTrack *genTrk = StPicoDst::mcTrack(igen);

        if(!isGenTrackGood(genTrk)) continue;

        TLorentzVector gen4Mom = genTrk->fourMomentum();

        double genPt = gen4Mom.Pt();
        double genEta = gen4Mom.Eta();
        double genPhi = gen4Mom.Phi();
        int genCharge = genTrk->charge();

        maxParticlePt = max(maxParticlePt, genPt);  

        TStarGenTrack *tsGenTrk = TStarArrays::addGenTrack();
        tsGenTrk->setVector(gen4Mom);
        tsGenTrk->setIndex(TStarArrays::numberOfGenTracks()-1);
        tsGenTrk->setCharge(genCharge);
        tsGenTrk->setGeantId(genTrk->geantId());
        tsGenTrk->setPdgId(genTrk->pdgId());
        tsGenTrk->setIdVtxStart(genTrk->idVtxStart());
        tsGenTrk->setIdVtxEnd(genTrk->idVtxStop());

        genTrackIndexMap[igen] = TStarArrays::numberOfGenTracks() - 1;

        if(doTrackDebug){
            cout<<"Made StPicoMcTrack into TStarGenTrack! comparing"<<endl;
            cout<<"StPicoMcTrack # "<<igen<<" mapped to TStarGenTrack # "<<genTrackIndexMap[igen]<<endl;
            tsGenTrk->Print();
            cout<<"Adding to gen track array..."<<endl;
        }

        if(doJetAnalysis){
            jetMaker->addConstituentVector(*tsGenTrk);
            if((genPt > jetConstituentMinPt) && doJetDebug){
                cout<<"StPicoMcTrack: "<<igen<<" Pt: "<<genPt<<" eta: "<<genEta<<" phi: "<<genPhi<<endl;
                cout<<"TStarGenTrack:"<<tsGenTrk->index()<<" Pt: "<<tsGenTrk->pt()<<" eta: "<<tsGenTrk->eta()<<" phi: "<<tsGenTrk->phi()<<endl;
            }
        }

        fillHist1D("hGenTrackPt", genPt, Wt);
        fillHist1D("hGenTrackEta", genEta, Wt);
        fillHist1D("hGenTrackPhi", tsGenTrk->phi(), Wt);
        fillHist2D("h2GenTrackEtavPhi", tsGenTrk->phi(), genEta, genPt*Wt);

    }//end gen track loop...

    fillHist2D("h2CentvMaxGenTrackPt", centscaled, maxParticlePt, Wt);
    if(doTrackDebug)cout<<"Final Max gen track Pt: "<<maxParticlePt<<endl;
    TStarArrays::getEvent()->setMaxGenTrackPt(maxParticlePt);
    if(doTrackDebug)cout<<"********** END StPicoAnalysisGenMaker::runOverGenTracks() done************"<<endl;
}

bool StPicoAnalysisGenMaker::isGenTrackGood(StPicoMcTrack* genTrk){
    if(doTrackDebug)cout<<"**********StPicoAnalysisGenMaker::isGenTrackGood()************"<<endl;

    if(!genTrk){
        if(doTrackDebug)cout<<"StPicoMcTrack pointer is null!"<<endl;
        return false;
    }fillHist1D("hGenTrackStats", 0);

    unsigned int idVtxStop = genTrk->idVtxStop();
    if(idVtxStop > 0){
        if(doTrackDebug)
            cout<<"Gen Track is not final state!"<<endl;
        return false;
    }fillHist1D("hGenTrackStats", 1);

    TVector3 genMom = genTrk->p();
    double genPt = genMom.Pt();
    double genEta = genMom.Eta();
    double genPhi = genMom.Phi();
//Track Pt:
    if(genPt < trackPtMin){
        if(doTrackDebug)
            cout<<"GenTrack failed pt cut: "<<genPt<<endl;
        return false;
    }fillHist1D("hGenTrackStats", 2);
//Track Eta:
    if((genEta < trackEtaMin) || (genEta > trackEtaMax)){
        if(doTrackDebug)
            cout<<"GenTrack failed eta cut: "<<genEta<<endl;
        return false;
    }fillHist1D("hGenTrackStats", 3);

    fillHist1D("hGenTrackStats", 4);
    if(doTrackDebug){
        cout<<"********** END StPicoAnalysisGenMaker::isGenTrackGood() done************"<<endl;
        cout<<"Gen track accepted: "<<" Pt: "<<genPt<<" eta: "<<genEta<<" phi: "<<genPhi<<endl;
        if(genPt > maxParticlePt)
            cout<<"Max gen track Pt changed from: "<<maxParticlePt<<" to "<<genPt<<endl;  
    }
    return true;
}

void StPicoAnalysisGenMaker::declareStatHistos(){
    if(iMaker == 0)StPicoAnalysisBaseMaker::declareStatHistos(); 
    addHist1D("hGenTrackStats", "GenTrack Statistics", 10, -0.5, 9.5);
    getHist1D("hGenTrackStats")->GetXaxis()->SetBinLabel(1, "ALL");
    getHist1D("hGenTrackStats")->GetXaxis()->SetBinLabel(2, "FINAL STATE");
    getHist1D("hGenTrackStats")->GetXaxis()->SetBinLabel(3, "PT PASS");
    getHist1D("hGenTrackStats")->GetXaxis()->SetBinLabel(4, "ETA PASS");
    getHist1D("hGenTrackStats")->GetXaxis()->SetBinLabel(5, "GOOD");
}
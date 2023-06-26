#define TStarJetEvent_cxx

#include "TStarJetEvent.h"
#include "TStarJet.h"
#include "TClonesArray.h"

using namespace std;

TStarJetEvent::TStarJetEvent(){
    Jets = new TClonesArray("TStarJet", 50);
}

TStarJetEvent::TStarJetEvent(float rho, float sigma, int njets) {
    _Rho = rho;
    _Sigma = sigma;
    _NJets = njets;
    Jets = new TClonesArray("TStarJet", 50);
}

TStarJetEvent::TStarJetEvent(const TStarJetEvent& je) {
    _Rho = je._Rho;
    _Sigma = je._Sigma;
    _NJets = je._NJets;
    Jets = new TClonesArray("TStarJet", 50);
    for(int i = 0; i < je.Jets->GetEntriesFast(); ++i){
        TStarJet* _jet = static_cast<TStarJet*>(je.Jets->At(i));
        new((*Jets)[i]) TStarJet(*_jet);
    }
}

TStarJetEvent::~TStarJetEvent(){
    if(!TestBit(kIsOnHeap)){
        delete Jets;
    }
}

TStarJet* TStarJetEvent::addJet(){
    int index = Jets->GetEntriesFast();
    TStarJet* jet = static_cast<TStarJet*>(Jets->ConstructedAt(index));
    return jet;
}

void TStarJetEvent::addJet(const TStarJet& j){
    new((*Jets)[Jets->GetEntriesFast()]) TStarJet(j);
}

void TStarJetEvent::clearJetArray(){
    Jets->Clear();
}




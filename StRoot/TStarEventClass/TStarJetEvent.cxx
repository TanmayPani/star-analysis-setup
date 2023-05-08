#define TStarJetEvent_cxx

#include "FJ_includes.h"

#include "TStarJetEvent.h"
#include "TStarJet.h"
#include "TStarEvent.h"
#include "TClonesArray.h"

using namespace fastjet;

using namespace std;

TStarJetEvent::TStarJetEvent(){
    Jets = new TClonesArray("TStarJet", 50);
}

TStarJetEvent::TStarJetEvent(int runid, int eventid){
    _RunID = runid;
    _EventID = eventid;
    Jets = new TClonesArray("TStarJet", 50);
}

TStarJetEvent::TStarJetEvent(TStarEvent *Ev){
    _RunID = Ev->RunNumber();
    _EventID = Ev->EventNumber();
    Jets = new TClonesArray("TStarJet", 50);
}

TStarJetEvent::~TStarJetEvent(){
    if(!TestBit(kIsOnHeap)){
        delete Jets;
    }
}

TStarJet* TStarJetEvent::AddJet(PseudoJet& _jet){
    if(_jet.is_pure_ghost())return nullptr;
    int index = Jets->GetEntriesFast();
    TStarJet* jet = static_cast<TStarJet*>(Jets->ConstructedAt(index));
    jet->SetJet(_jet);
    return jet;
}

void TStarJetEvent::ClearJetArray(){
    Jets->Clear();
}




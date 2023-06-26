#define TStarJet_cxx

#include "TStarJet.h"
#include "TStarJetConstituent.h"
#include "TClonesArray.h"

ClassImp(TStarJet);

using namespace std;

TStarJet::TStarJet(){
    Constituents = new TClonesArray("TStarJetConstituent", 50);
}

TStarJet::TStarJet(unsigned int i, float px, float py, float pz, float e) : 
TStarVector(px, py, pz, e){
    _Index = i;
    Constituents = new TClonesArray("TStarJetConstituent", 50);
}

TStarJet::TStarJet(const TStarJet& j) : TStarVector(j) {
    _Index = j._Index;
    _PtSub = j._PtSub;
    _A = j._A;
    _Ax = j._Ax;
    _Ay = j._Ay;
    _Az = j._Az;
    _Rho = j._Rho;
    _Sigma = j._Sigma;
    Constituents = new TClonesArray("TStarJetConstituent", 50);
    for(int i = 0; i < j.Constituents->GetEntriesFast(); ++i){
        TStarJetConstituent* _constit = static_cast<TStarJetConstituent*>(j.Constituents->At(i));
        TStarJetConstituent* _newconstit = static_cast<TStarJetConstituent*>(Constituents->ConstructedAt(i));
        _newconstit = new TStarJetConstituent(*_constit);
    }
}

TStarJet::~TStarJet(){
    if(!TestBit(kIsOnHeap)){
        delete Constituents;
    }
}

TStarJetConstituent* TStarJet::addConstituent(){
    //if(_con.is_pure_ghost())return nullptr;
    int index = Constituents->GetEntriesFast();
    TStarJetConstituent* _constit 
            = static_cast<TStarJetConstituent*>(Constituents->ConstructedAt(index));
    return _constit;
}

void TStarJet::addConstituent(const TStarJetConstituent& c){
    new((*Constituents)[Constituents->GetEntriesFast()]) TStarJetConstituent(c);
}

void TStarJet::clearConstituentArray(){
    Constituents->Clear();
}
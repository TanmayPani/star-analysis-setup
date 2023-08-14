#define TStarMixedEventArray_cxx

#include <cassert>
#include <iostream>

#include "TStarMixedEventArray.h"

#include "TClonesArray.h"
#include "TTree.h"

#include "TStarTrack.h"

ClassImp(TStarMixedEventArray);

using namespace std;

int TStarMixedEventArray::bufferSize = 65536;
int TStarMixedEventArray::splitLevel = 99;

TStarMixedEventArray* TStarMixedEventArray::_instance = nullptr;

TStarMixedEventArray* TStarMixedEventArray::instance() {
    if(!instance) _instance = new TStarMixedEventArray();
    return _instance;
}

unsigned int TStarMixedEventArray::maxBinNTracks = 1000;

map<string, TClonesArray*> TStarMixedEventArray::Arrays;


TStarMixedEventArray::TStarMixedEventArray() : TClass("TStarMixedEventArray") {
    _instance = this;
}

TStarMixedEventArray::~TStarMixedEventArray() {

}

void TStarMixedEventArray::ignoreTObjectStreamer() {
    TStarVector::Class()->IgnoreTObjectStreamer();
}

void TStarMixedEventArray::setBranch(TTree* tree) {
    for(auto& arr : Arrays){
        tree->Branch(arr.first.c_str(), &(arr.second), bufferSize, splitLevel);
    }
}

void TStarMixedEventArray::clearArrays() {
    for(auto& arr : Arrays){
        arr.second->Clear();
    }
}

void TStarMixedEventArray::addMixedEvent(int refBin, int zVtxBin, int trkPtBin) {
    string name = "mixedEvents_"+to_string(refBin)+"_"+to_string(zVtxBin)+"_"+to_string(trkPtBin);
    cout<<"Adding mixed event array: "<<name<<endl;
    Arrays[name] = new TClonesArray("TStarTrack", maxBinNTracks);
}

void TStarMixedEventArray::addAll(unsigned int nRefBins, unsigned int nZVtxBins, unsigned int nTrkPtBins){
    for(unsigned int i = 1; i <= nRefBins; ++i){
        for(unsigned int j = 1; j <= nZVtxBins; ++j){
            for(unsigned int k = 1; k <= nTrkPtBins; ++k){
                addMixedEvent(i, j, k);
            }
        }
    }
}

bool TStarMixedEventArray::hasMixedEvent(int refBin, int zVtxBin, int trkPtBin){
    string name = "mixedEvents_"+to_string(refBin)+"_"+to_string(zVtxBin)+"_"+to_string(trkPtBin);
    auto arrFinder = Arrays.find(name);
    return arrFinder != Arrays.end();
}

TStarTrack* TStarMixedEventArray::addTrack(int refBin, int zVtxBin, int trkPtBin){
    string name = "mixedEvents_"+to_string(refBin)+"_"+to_string(zVtxBin)+"_"+to_string(trkPtBin);
    auto arrFinder = Arrays.find(name);
    assert((void(name+"Array not found! \n"), arrFinder != Arrays.end()));

    int index = arrFinder->second->GetEntriesFast();
    if(index >= maxBinNTracks) return nullptr;

    return static_cast<TStarTrack*>(arrFinder->second->ConstructedAt(index));
}

void TStarMixedEventArray::addTrack(int refBin, int zVtxBin, int trkPtBin, const TStarTrack& track){
    TStarTrack* newTrack = addTrack(refBin, zVtxBin, trkPtBin);
    if(newTrack == nullptr) return;
    newTrack->setTrack(track);
    cout<<"Added track to mixed event array: "<<refBin<<" "<<zVtxBin<<" "<<trkPtBin<<" "<<newTrack->pt()<<endl;
}

TStarTrack* TStarMixedEventArray::getTrack(int refBin, int zVtxBin, int trkPtBin, unsigned int i){
    string name = "mixedEvents_"+to_string(refBin)+"_"+to_string(zVtxBin)+"_"+to_string(trkPtBin);
    auto arrFinder = Arrays.find(name);
    assert((void(name+"Array not found! \n"), arrFinder != Arrays.end()));
    assert((void("Index out of range! \n"), i < arrFinder->second->GetEntriesFast()));
    return static_cast<TStarTrack*>(arrFinder->second->At(i));
}

unsigned int TStarMixedEventArray::nMixedEventTracks(int refBin, int zVtxBin, int trkPtBin){
    string name = "mixedEvents_"+to_string(refBin)+"_"+to_string(zVtxBin)+"_"+to_string(trkPtBin);
    auto arrFinder = Arrays.find(name);
    assert((void(name+"Array not found! \n"), arrFinder != Arrays.end()));

    return arrFinder->second->GetEntriesFast();
}

bool TStarMixedEventArray::isFilled(int refBin, int zVtxBin, int trkPtBin){
    string name = "mixedEvents_"+to_string(refBin)+"_"+to_string(zVtxBin)+"_"+to_string(trkPtBin);
    auto arrFinder = Arrays.find(name);
    if(arrFinder != Arrays.end()){return false;}

    return arrFinder->second->GetEntriesFast() >= maxBinNTracks;
}

bool TStarMixedEventArray::isFilled(){
    for(auto& arr : Arrays){
        if(arr.second->GetEntriesFast() < maxBinNTracks) return false;
    }
    return true;
}


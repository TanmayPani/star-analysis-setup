#define TStarArrays_cxx

#include <cassert>

#include "TStarArrays.h"

#include "TClonesArray.h"
#include "TTree.h"

#include "TStarEvent.h"
#include "TStarJet.h"
#include "TStarTrack.h"
#include "TStarTower.h"
#include "TStarGenTrack.h"
#include "StRoot/StPythiaEventMaker/StPythiaEvent.h"

ClassImp(TStarArrays);

using namespace std;

map<string, TClonesArray*> TStarArrays::Arrays;

TStarEvent* TStarArrays::event = new TStarEvent();

map<string, string> TStarArrays::Types = {
    {"event", "TStarEvent"}, {"pythiaEvent", "StPythiaEvent"},
    {"tracks", "TStarTrack"}, {"towers", "TStarTower"},
    {"jets", "TStarJet"}, {"genTracks", "TStarGenTrack"},
    {"genJets", "TStarJet"}
};

map<string, int> TStarArrays::Sizes = {
    {"event", 1}, {"pythiaEvent", 1},
    {"tracks", 10000}, {"towers", 10000},
    {"jets", 100}, {"genTracks", 10000},
    {"genJets", 100}
};

//map<string, function<double(TObject*)>> TStarArrays::varNames = {
//    {"Pt", [](TObject* obj){return static_cast<TStarVector*>(obj)->pt();}},
//    {"Eta", [](TObject* obj){return static_cast<TStarVector*>(obj)->eta();}},
//    {"Phi", [](TObject* obj){return static_cast<TStarVector*>(obj)->phi();}}
//};

int TStarArrays::bufferSize = 65536;
int TStarArrays::splitLevel = 99;

TStarArrays* TStarArrays::_instance = nullptr;

TStarArrays* TStarArrays::instance() {
    if(!instance) _instance = new TStarArrays();
    return _instance;
}

TStarArrays::TStarArrays() : TClass("TStarArrays") {
    _instance = this;
}

TStarArrays::~TStarArrays() {
    _instance = nullptr;
}

void TStarArrays::addArray(string name) {
    auto arrFinder = Types.find(name);
    assert(arrFinder != Types.end());
    
    Arrays[name] = new TClonesArray(arrFinder->second.c_str(), Sizes[name]);
}

void TStarArrays::ignoreTObjectStreamer() {
    TStarEvent::Class()->IgnoreTObjectStreamer();
    StPythiaEvent::Class()->IgnoreTObjectStreamer();
    TStarVector::Class()->IgnoreTObjectStreamer();
}

void TStarArrays::setBranch(TTree* tree) {
    for(auto& arr : Arrays){
        if(arr.first == "event"){
            tree->Branch("event", &event, bufferSize, splitLevel);
            continue;
        }
        tree->Branch(arr.first.c_str(), &(arr.second), bufferSize, splitLevel);
    }
}

void TStarArrays::setEvent(){
    event->setEvent(*(getEvent()));
}

void TStarArrays::clearArrays() {
    for(auto& arr : Arrays){
        arr.second->Clear();
    }
}

TObject* TStarArrays::addArrayElement(const string& name){
    auto arrFinder = Arrays.find(name);
    assert((void(name+"Array not found! \n"), arrFinder != Arrays.end()));

    int index = arrFinder->second->GetEntriesFast();
    assert((void("Array "+name+" is full! \n"), index < Sizes[name]));

    return arrFinder->second->ConstructedAt(index);
}
TStarEvent* TStarArrays::addEvent(){return static_cast<TStarEvent*>(addArrayElement("event"));}
StPythiaEvent* TStarArrays::addPythiaEvent(){return static_cast<StPythiaEvent*>(addArrayElement("pythiaEvent"));} 
TStarTrack* TStarArrays::addTrack(){return static_cast<TStarTrack*>(addArrayElement("tracks"));}
TStarTower* TStarArrays::addTower(){return static_cast<TStarTower*>(addArrayElement("towers"));}
TStarJet* TStarArrays::addJet(){return static_cast<TStarJet*>(addArrayElement("jets"));}
TStarGenTrack* TStarArrays::addGenTrack(){return static_cast<TStarGenTrack*>(addArrayElement("genTracks"));}
TStarJet* TStarArrays::addGenJet(){return static_cast<TStarJet*>(addArrayElement("genJets"));}

unsigned int TStarArrays::nArrayElements(const string& name){
    auto arrFinder = Arrays.find(name);
    assert((void(name+"Array not found! \n"), arrFinder != Arrays.end()));

    return arrFinder->second->GetEntriesFast();
}

TObject* TStarArrays::getArrayElement(const string& name, unsigned int i){
    auto arrFinder = Arrays.find(name);
    assert((void(name+"Array not found! \n"), arrFinder != Arrays.end()));
    assert((void("Index out of range! \n"), i < arrFinder->second->GetEntriesFast()));
    return arrFinder->second->At(i);
}
TStarEvent* TStarArrays::getEvent(){return static_cast<TStarEvent*>(getArrayElement("event", 0));}
StPythiaEvent* TStarArrays::getPythiaEvent(){return static_cast<StPythiaEvent*>(getArrayElement("pythiaEvent", 0));}
TStarTrack* TStarArrays::getTrack(unsigned int i){return static_cast<TStarTrack*>(getArrayElement("tracks", i));}
TStarTower* TStarArrays::getTower(unsigned int i){return static_cast<TStarTower*>(getArrayElement("towers", i));}
TStarJet* TStarArrays::getJet(unsigned int i){return static_cast<TStarJet*>(getArrayElement("jets", i));}
TStarGenTrack* TStarArrays::getGenTrack(unsigned int i){return static_cast<TStarGenTrack*>(getArrayElement("genTracks", i));}
TStarJet* TStarArrays::getGenJet(unsigned int i){return static_cast<TStarJet*>(getArrayElement("genJets", i));}
TStarVector* TStarArrays::getVector(const string& arrName, unsigned int i){
    TStarVector* vec = dynamic_cast<TStarVector*>(getArrayElement(arrName, i));
    assert((void("Object is not a TStarVector! \n"), vec != nullptr));
    return vec;
}

bool TStarArrays::hasEvent(){
    auto arrFinder = Arrays.find("event");
    if(arrFinder == Arrays.end()) return false;
    return arrFinder->second->GetEntriesFast() > 0;
}

bool TStarArrays::hasArray(const string& name){
    auto arrFinder = Arrays.find(name);
    return arrFinder != Arrays.end();
}

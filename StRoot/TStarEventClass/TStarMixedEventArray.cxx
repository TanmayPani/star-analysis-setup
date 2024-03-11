#define TStarMixedEventArray_cxx

#include <cassert>
#include <iostream>

#include "TStarMixedEventArray.h"

#include "TClonesArray.h"
#include "TTree.h"

#include "TStarMixedEvent.h"
#include "TStarMixedEventTrack.h"

ClassImp(TStarMixedEventArray);

using namespace std;

map<string, TClonesArray*> TStarMixedEventArray::Arrays;

TStarMixedEvent* TStarMixedEventArray::event = new TStarMixedEvent();

map<string, string> TStarMixedEventArray::Types = {
    {"event", "TStarMixedEvent"}, {"tracks", "TStarMixedEventTrack"}
};

map<string, int> TStarMixedEventArray::Sizes = {
    {"event", 1}, {"tracks", 10000}
};

int TStarMixedEventArray::bufferSize = 65536;
int TStarMixedEventArray::splitLevel = 99;

TStarMixedEventArray* TStarMixedEventArray::_instance = nullptr;

TStarMixedEventArray* TStarMixedEventArray::instance() {
    if(!instance) _instance = new TStarMixedEventArray();
    return _instance;
}

TStarMixedEventArray::TStarMixedEventArray() : TClass("TStarMixedEventArray") {
    _instance = this;
}

TStarMixedEventArray::~TStarMixedEventArray() {
    _instance = nullptr;

}

void TStarMixedEventArray::addArray(string name) {
    auto arrFinder = Types.find(name);
    assert(arrFinder != Types.end());
    
    Arrays[name] = new TClonesArray(arrFinder->second.c_str(), Sizes[name]);
}

void TStarMixedEventArray::ignoreTObjectStreamer() {
    TStarMixedEvent::Class()->IgnoreTObjectStreamer();
    TStarMixedEventTrack::Class()->IgnoreTObjectStreamer();
}

void TStarMixedEventArray::setBranch(TTree* tree) {
    tree->Branch("event", &event, bufferSize, splitLevel);
    tree->Branch("tracks", Arrays["tracks"], bufferSize, splitLevel);
}

void TStarMixedEventArray::clearArrays() {
    for(auto& arr : Arrays){
        arr.second->Clear();
    }
}

void TStarMixedEventArray::setEvent(){
    event->setEvent(*(getEvent()));
}

TObject* TStarMixedEventArray::addArrayElement(const string& name){
    auto arrFinder = Arrays.find(name);
    assert((void(name+"Array not found! \n"), arrFinder != Arrays.end()));

    int index = arrFinder->second->GetEntriesFast();
    assert((void("Array "+name+" is full! \n"), index < Sizes[name]));

    return arrFinder->second->ConstructedAt(index);
}

unsigned int TStarMixedEventArray::nArrayElements(const string& name){
    auto arrFinder = Arrays.find(name);
    assert((void(name+"Array not found! \n"), arrFinder != Arrays.end()));

    return arrFinder->second->GetEntriesFast();
}

TObject* TStarMixedEventArray::getArrayElement(const string& name, unsigned int i){
    auto arrFinder = Arrays.find(name);
    assert((void(name+"Array not found! \n"), arrFinder != Arrays.end()));
    assert((void("Index out of range! \n"), i < arrFinder->second->GetEntriesFast()));
    return arrFinder->second->At(i);
}

bool TStarMixedEventArray::hasEvent(){
    auto arrFinder = Arrays.find("event");
    if(arrFinder == Arrays.end()) return false;
    return arrFinder->second->GetEntriesFast() > 0;
}

bool TStarMixedEventArray::hasArray(const string& name){
    auto arrFinder = Arrays.find(name);
    return arrFinder != Arrays.end();
}
TStarMixedEvent* TStarMixedEventArray::addEvent(){return static_cast<TStarMixedEvent*>(addArrayElement("event"));}
TStarMixedEventTrack* TStarMixedEventArray::addTrack(){return static_cast<TStarMixedEventTrack*>(addArrayElement("tracks"));}

TStarMixedEvent* TStarMixedEventArray::getEvent(){return static_cast<TStarMixedEvent*>(getArrayElement("event", 0));}
TStarMixedEventTrack* TStarMixedEventArray::getTrack(unsigned int i){return static_cast<TStarMixedEventTrack*>(getArrayElement("tracks", i));}


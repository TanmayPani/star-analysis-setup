#ifndef TStarMixedEventArray_h
#define TStarMixedEventArray_h

#include "TClass.h"

#include <map>
#include <string>
//#include <functional>

class TClonesArray;
class TTree;

class TStarMixedEventTrack;
class TStarMixedEvent;

class TStarMixedEventArray : public TClass {
public:
    TStarMixedEventArray();
    virtual ~TStarMixedEventArray();

    static void addArray(std::string name);
    static void setBranch(TTree* tree);
    static void clearArrays();
    static void ignoreTObjectStreamer();

    static TStarMixedEvent* addEvent();
    static TStarMixedEventTrack* addTrack();

    static TStarMixedEvent* getEvent();
    static TStarMixedEventTrack* getTrack(unsigned int i);

    static bool hasEvent(); 
    static unsigned int numberOfTracks(){return nArrayElements("tracks");}

    static bool hasArray(const std::string& name);
    static TObject* addArrayElement(const std::string& name);
    static unsigned int nArrayElements(const std::string& name);
    static TObject* getArrayElement(const std::string& name, unsigned int i);

    static void setBufferSize(int size){bufferSize = size;}
    static void setSplitLevel(int level){splitLevel = level;}

    static void setEvent();

    static TStarMixedEventArray* instance();

private:
    static std::map<std::string, TClonesArray*> Arrays;
    static std::map<std::string, std::string> Types;
    static std::map<std::string, int> Sizes;

    static TStarMixedEvent* event;

    static int bufferSize;
    static int splitLevel;

    static TStarMixedEventArray* _instance;

    ClassDef(TStarMixedEventArray, 1)
};



#endif

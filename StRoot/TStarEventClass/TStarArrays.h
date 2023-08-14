#ifndef TStarArrays_h
#define TStarArrays_h

#include "TClass.h"

#include <map>
#include <string>
//#include <functional>

class TClonesArray;
class TTree;

class TObject;
class TStarEvent;
class TStarJet;
class TStarTrack;
class TStarTower;
class TStarGenTrack;
class TStarVector;

class StPythiaEvent;

class TStarArrays : public TClass {
public:
    TStarArrays();
    virtual ~TStarArrays();

    static void addArray(std::string name);
    static void setBranch(TTree* tree);
    static void clearArrays();
    static void ignoreTObjectStreamer();

    static TStarEvent* addEvent();
    static StPythiaEvent* addPythiaEvent();
    static TStarTrack* addTrack();
    static TStarTower* addTower();
    static TStarJet* addJet();
    static TStarGenTrack* addGenTrack();
    static TStarJet* addGenJet();

    static bool hasEvent(); 
    static unsigned int numberOfTracks(){return nArrayElements("tracks");}
    static unsigned int numberOfTowers(){return nArrayElements("towers");}
    static unsigned int numberOfJets(){return nArrayElements("jets");}
    static unsigned int numberOfGenTracks(){return nArrayElements("genTracks");}
    static unsigned int numberOfGenJets(){return nArrayElements("genJets");}
    
    static TStarEvent* getEvent();
    static StPythiaEvent* getPythiaEvent();
    static TStarTrack* getTrack(unsigned int i);
    static TStarTower* getTower(unsigned int i);
    static TStarJet* getJet(unsigned int i);
    static TStarGenTrack* getGenTrack(unsigned int i);
    static TStarJet* getGenJet(unsigned int i);
    static TStarVector* getVector(const std::string& arrName, unsigned int i);    

    //static TClonesArray* getArray(const std::string& name);

    static bool hasArray(const std::string& name);

    static TObject* addArrayElement(const std::string& name);

    static unsigned int nArrayElements(const std::string& name);
    static TObject* getArrayElement(const std::string& name, unsigned int i);

    static void setBufferSize(int size){bufferSize = size;}
    static void setSplitLevel(int level){splitLevel = level;}

    static void setEvent();

    static TStarArrays* instance();

private:
    static std::map<std::string, TClonesArray*> Arrays;

    static TStarEvent* event;

    static std::map<std::string, std::string> Types;
    static std::map<std::string, int> Sizes;

    static int bufferSize;
    static int splitLevel;

    static TStarArrays* _instance;

    //static std::map<std::string, std::function<double(TObject*)>> varNames;

    ClassDef(TStarArrays, 1)
};



#endif

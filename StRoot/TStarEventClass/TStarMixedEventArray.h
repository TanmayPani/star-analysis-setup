#ifndef TStarMixedEventArray_h
#define TStarMixedEventArray_h

#include "TClass.h"

#include <map>
#include <string>
//#include <functional>

class TClonesArray;
class TTree;

class TStarTrack;

class TStarMixedEventArray : public TClass {
public:
    TStarMixedEventArray();
    virtual ~TStarMixedEventArray();

    static void setBranch(TTree* tree);
    static void clearArrays();
    static void ignoreTObjectStreamer();

    static void addMixedEvent(int refBin, int zVtxBin, int trkPtBin);
    static void addAll(unsigned int nRefBins, unsigned int nZVtxBins, unsigned int nTrkPtBins);
    static bool hasMixedEvent(int refBin, int zVtxBin, int trkPtBin); 

    static TStarTrack* addTrack(int refBin, int zVtxBin, int trkPtBin);
    static void addTrack(int refBin, int zVtxBin, int trkPtBin, const TStarTrack& track);
    static TStarTrack* getTrack(int refBin, int zVtxBin, int trkPtBin, unsigned int i);
    static unsigned int nMixedEventTracks(int refBin, int zVtxBin, int trkPtBin);

    static void setMaxBinNTracks(unsigned int n) { maxBinNTracks = n; }
    static unsigned int getMaxBinNTracks() { return maxBinNTracks; }

    static bool isFilled(int refBin, int zVtxBin, int trkPtBin);
    static bool isFilled();

    static void setBufferSize(int size){bufferSize = size;}
    static void setSplitLevel(int level){splitLevel = level;}

    static TStarMixedEventArray* instance();

private:
    static unsigned int maxBinNTracks;
    static std::map<std::string, TClonesArray*> Arrays;

    static int bufferSize;
    static int splitLevel;
    static TStarMixedEventArray* _instance;

    ClassDef(TStarMixedEventArray, 1)
};



#endif

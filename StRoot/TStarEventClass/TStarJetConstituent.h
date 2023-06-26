#ifndef TStarJetConstituent_h
#define TStarJetConstituent_h

#include "TStarVector.h"

class TStarJetConstituent : public TStarVector{
public:
    TStarJetConstituent(unsigned int i);
    TStarJetConstituent(unsigned int i, signed char ch, float px, float py, float pz, float e);
    TStarJetConstituent(unsigned int i, signed char ch, TStarVector& v);
    TStarJetConstituent(const TStarJetConstituent& c);

    virtual ~TStarJetConstituent();

    unsigned int index(){return _Index;}
    signed char charge(){return _Charge;}

    void setIndex(unsigned int i){_Index = i;}
    void setCharge(signed char ch){_Charge = ch;}

    unsigned int _Index = 0;
    signed char _Charge = 0;

    ClassDef(TStarJetConstituent, 1);
};

#endif
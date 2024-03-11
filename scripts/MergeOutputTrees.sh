#!/bin/sh

OUTDIR="/gpfs01/star/pwg/tpani/output/AuAu_200_production_mid_2014_P18ih_SL20d/20230814/Histograms"

RUNLIST="RUNLISTS/AuAu_200_production_mid_2014_GOOD_woNoHT.list"

TREETOMERGE="hist"

MERGEDDIR="Histograms_20230814"
mkdir -p  $MERGEDDIR

ITERATION=0
while read RUNNUMBER && [ $ITERATION -lt 1000 ]; do 
    ITERATION=$((ITERATION+1))
    hadd -n 50 $MERGEDDIR/$RUNNUMBER.$TREETOMERGE.root $OUTDIR/$RUNNUMBER_*.$TREETOMERGE.root
    #rm -rf $OUTDIR/$JOBUSERNAME/out/${TREETOMERGE}s/${TREETOMERGE}_${RUNNUMBER}_*.root
done < $RUNLIST


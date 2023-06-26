#!/bin/sh

OUTDIR="/gpfs01/star/pwg/tpani/output/PicosToTree2"
MYJOBNAME="20230626"
PROD="P18ih"
LIB="SL20d"
TRGSETUP="AuAu_200_production_mid_2014"
RUNLIST="RUNLISTS/AuAu_200_production_mid_2014_GOOD.list"

JOBUSERNAME="${TRGSETUP}_${PROD}_${LIB}_${MYJOBNAME}"

TREETOMERGE="Histograms"
ITERATION=0
while read RUNNUMBER && [ $ITERATION -lt 1000 ]; do 
    ITERATION=$((ITERATION+1))
    hadd -n 50 $OUTDIR/$JOBUSERNAME/out/${TREETOMERGE}_$RUNNUMBER.root $OUTDIR/$JOBUSERNAME/out/${TREETOMERGE}/${TREETOMERGE}_${RUNNUMBER}_*.root
done < $RUNLIST

hadd -n 50 Histograms.root $OUTDIR/$JOBUSERNAME/out/${TREETOMERGE}_*.root 

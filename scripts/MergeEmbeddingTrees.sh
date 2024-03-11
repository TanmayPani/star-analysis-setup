#!/bin/sh

OUTDIR="/gpfs01/star/pwg/tpani/output/Pythia6Embedding_pp200_production_2012_P12id_SL12d_20212001_MuToTree20231019"

ptHats=("55_-1" "45_55" "35_45" "25_35" "20_25" "15_20" "11_15")

TREETOMERGE="tree"

MERGEDDIR="$OUTDIR/merged_MuToTree20231019"
mkdir -p $MERGEDDIR

for pth in "${ptHats[@]}"; do
    hadd -n 50 $MERGEDDIR/pt${pth}.$TREETOMERGE.root  $OUTDIR/pt${pth}_0/$TREETOMERGE/*.$TREETOMERGE.root
done
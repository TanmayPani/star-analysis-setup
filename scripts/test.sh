#!/bin/sh

FILE="/star/embed/embedding/pp200_production_2012/v2/Pythia6_pt55_-1_100_20212001/P12id.SL12d/2012/047/13047003/st_zerobias_adc_13047003_raw_3600001_r0.MuDst.root"
cp $FILE .
root4star -l -b -q genDst.C\(-1,\"picoDst,PicoVtxMode:PicoVtxVpdOrDefault,PicoCovMtxMode:PicoCovMtxWrite\",\"st_zerobias_adc_13047003_raw_3600001_r0.MuDst.root\"\)

root -l -b -q readPicoDstAnalysisMaker.C\(\"st_zerobias_adc_13047003_raw_3600001_r0.MuDst.root\",\"dummy.root\",1000000000\)
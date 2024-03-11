#!/bin/sh

MAINPWD="/gpfs01/star/pwg/tpani/output/Pythia6Embedding_pp200_production_2012_P12id_SL12d_20212001_MuToPico20231018/pt55_-1_0/out"
FILELIST="embedding_pt55_-1_pp200_production_2012.list"

find $MAINPWD -name "*.picoDst.root" > $FILELIST 
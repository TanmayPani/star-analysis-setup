#!/bin/sh

echo "###############################################################################################"
echo "running kinit: "
eval kinit $USER
echo "running aklog: "
eval aklog
echo "###############################################################################################"

OUTDIR="/gpfs01/star/pwg/tpani/output/PicosToTree2"
mkdir -p $OUTDIR

MYJOBNAME=$(date +%Y%m%d)
NMAXPROCESSFILES="10"
SIMULATE="false"

PROD="P18ih"
LIB="SL20d"
TRGSETUP="AuAu_200_production_mid_2014"
RUNLIST="RUNLISTS/AuAu_200_production_mid_2014_GOOD.list"

if [[ $SIMULATE == "true" ]]; then
	NFILES="20"
elif [[ $SIMULATE == "false" ]]; then
	NFILES="all"
else
	NFILES="all"
fi

JOBUSERNAME="${TRGSETUP}_${PROD}_${LIB}_${MYJOBNAME}"
mkdir -p $OUTDIR/$JOBUSERNAME

mkdir -p JOBXML_FILES/${JOBUSERNAME}

ITERATION=0

while read RUNNUMBER && [ $ITERATION -lt 1000 ]; do #Set to some really big number for a real submission

	ITERATION=$((ITERATION+1))

	#mkdir -p $OUTDIR/$JOBUSERNAME/$RUNNUMBER
	mkdir -p $OUTDIR/$JOBUSERNAME/gen
	mkdir -p $OUTDIR/$JOBUSERNAME/log
	mkdir -p $OUTDIR/$JOBUSERNAME/out 
	mkdir -p $OUTDIR/$JOBUSERNAME/out/EventTrees
	mkdir -p $OUTDIR/$JOBUSERNAME/out/Histograms
	#mkdir -p $OUTDIR/$JOBUSERNAME/out/JetTrees 

	XMLSCRIPT="JOBXML_FILES/$JOBUSERNAME/SubmitJobs_$RUNNUMBER.xml"

cat> "$XMLSCRIPT" <<EOL
<?xml version="1.0" encoding="utf-8" ?> 
<job  name="D${MYJOBNAME}_R${RUNNUMBER}_" maxFilesPerProcess="$NMAXPROCESSFILES" fileListSyntax="xrootd" simulateSubmission="$SIMULATE" >

	<input URL="catalog:star.bnl.gov?production=$PROD,\
library=$LIB,\
trgsetupname=$TRGSETUP,\
filetype=daq_reco_picoDst,sname2=st_physics,\
runnumber=$RUNNUMBER,\
storage!=hpss" nFiles="$NFILES" />

	<stdout URL="file:$OUTDIR/$JOBUSERNAME/log/${RUNNUMBER}_\$JOBINDEX.log" />
	<stderr URL="file:$OUTDIR/$JOBUSERNAME/log/${RUNNUMBER}_\$JOBINDEX.err" />
	<output fromScratch="EventTree_${RUNNUMBER}_\$JOBINDEX.root" toURL="file:$OUTDIR/$JOBUSERNAME/out/EventTrees/" /> 
	<output fromScratch="Histograms_${RUNNUMBER}_\$JOBINDEX.root" toURL="file:$OUTDIR/$JOBUSERNAME/out/Histograms/" /> 
	<Generator> 
		<Location>$OUTDIR/$JOBUSERNAME/gen/</Location> 
	</Generator>

    <SandBox>
        <Package>
            <File>file:./readPicoDstAnalysisMaker.C</File>
            <File>file:./StRoot/</File>
            <File>file:./.sl73_gcc485/</File>
        </Package>
    </SandBox>

	<command>
		ln -s $FASTJET/include/fastjet
		ln -s $FASTJET/include/siscone

		starver new
		setenv FASTJET $FASTJET

	    root -b -q -l readPicoDstAnalysisMaker.C\(\"\$FILELIST\",\"${RUNNUMBER}_\$JOBINDEX.root\",1000000000\)
	   
	    unlink fastjet
	    unlink siscone   
	</command>

</job>
EOL
	star-submit $XMLSCRIPT
done < $RUNLIST

rm -r *.dataset *.session.xml

#bash concheck.sh $myDate $BNLusername

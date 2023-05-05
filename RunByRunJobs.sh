#!/bin/sh

JOBUSERNAME=$1

echo "###############################################################################################"
echo "running kinit: "
eval kinit $USER
echo "running aklog: "
eval aklog
echo "###############################################################################################"

OUTDIR="/gpfs01/star/pwg/tpani/output/PicosToTree"
#FASTJETINSTALL="/star/u/tpani/FastJet/fastjet-install"
mkdir -p $OUTDIR
mkdir -p $OUTDIR/$JOBUSERNAME

MYJOBNAME=$(date +%Y-%m-%d)
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

mkdir -p JOBXML_FILES/${JOBUSERNAME}_${MYJOBNAME}

ITERATION=0

while read RUNNUMBER && [ $ITERATION -lt 10 ]; do

	ITERATION=$((ITERATION+1))

	mkdir -p $OUTDIR/$JOBUSERNAME/$RUNNUMBER
	mkdir -p $OUTDIR/$JOBUSERNAME/$RUNNUMBER/gen
	mkdir -p $OUTDIR/$JOBUSERNAME/$RUNNUMBER/log
	mkdir -p $OUTDIR/$JOBUSERNAME/$RUNNUMBER/out 
	mkdir -p $OUTDIR/$JOBUSERNAME/$RUNNUMBER/out/EventTrees
	mkdir -p $OUTDIR/$JOBUSERNAME/$RUNNUMBER/out/JetTrees 

	XMLSCRIPT="JOBXML_FILES/${JOBUSERNAME}_${MYJOBNAME}/SubmitJobs_$RUNNUMBER.xml"

cat> "$XMLSCRIPT" <<EOL
<?xml version="1.0" encoding="utf-8" ?> 
<job  name="${MYJOBNAME}_${RUNNUMBER}" maxFilesPerProcess="$NMAXPROCESSFILES" fileListSyntax="xrootd" simulateSubmission="$SIMULATE" >

	<input URL="catalog:star.bnl.gov?production=$PROD,\
library=$LIB,\
trgsetupname=$TRGSETUP,\
filetype=daq_reco_picoDst,sname2=st_physics,\
runnumber=$RUNNUMBER,\
storage!=hpss" nFiles="$NFILES" />

	<stdout URL="file:$OUTDIR/$JOBUSERNAME/$RUNNUMBER/log/\$JOBID.log" />
	<stderr URL="file:$OUTDIR/$JOBUSERNAME/$RUNNUMBER/log/\$JOBID.err" />
	<output fromScratch="EventTree_${RUNNUMBER}_\$JOBINDEX.root" toURL="file:$OUTDIR/$JOBUSERNAME/$RUNNUMBER/out/EventTrees/" /> 
	<Generator> 
		<Location>$OUTDIR/$JOBUSERNAME/$RUNNUMBER/gen/</Location> 
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
done < $RUNLIST
	#star-submit $XMLSCRIPT

#bash concheck.sh $myDate $BNLusername

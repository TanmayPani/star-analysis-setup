#!/bin/sh

echo "###############################################################################################"
echo "running kinit: "
eval kinit $USER
echo "running aklog: "
eval aklog
echo "###############################################################################################"

OUTDIR="/gpfs01/star/pwg/$USER/output/"

MYJOBNAME=$(date +%Y%m%d)
NMAXPROCESSFILES="10"
SIMULATE="false"

PROD="P18ih"
LIB="SL20d"
TRGSETUP="AuAu_200_production_mid_2014"

RUNLIST="RUNLISTS/${TRGSETUP}_GOOD_woNoHT.list"

if [[ $SIMULATE == "true" ]]; then
	NFILES="20"
elif [[ $SIMULATE == "false" ]]; then
	NFILES="all"
else
	NFILES="all"
fi

$SYSTEM="${TRGSETUP}_${PROD}_${LIB}"

mkdir -p $OUTDIR
mkdir -p $OUTDIR/$SYSTEM
mkdir -p $OUTDIR/$SYSTEM/$MYJOBNAME

mkdir -p JOBXML_FILES
mkdir -p JOBXML_FILES/$SYSTEM
mkdir -p JOBXML_FILES/$SYSTEM/$MYJOBNAME

ITERATION=0

while read RUNNUMBER && [ $ITERATION -lt 1000 ]; do #Set to some really big number for a real submission

	ITERATION=$((ITERATION+1))

	mkdir -p $OUTDIR/$SYSTEM/$MYJOBNAME/gen
	mkdir -p $OUTDIR/$SYSTEM/$MYJOBNAME/log
	mkdir -p $OUTDIR/$SYSTEM/$MYJOBNAME/Events
	mkdir -p $OUTDIR/$SYSTEM/$MYJOBNAME/Histograms
	mkdir -p $OUTDIR/$SYSTEM/$MYJOBNAME/MixedEvents

	XMLSCRIPT="JOBXML_FILES/$SYSTEM/$MYJOBNAME/SubmitJobs_$RUNNUMBER.xml"

cat> "$XMLSCRIPT" <<EOL
<?xml version="1.0" encoding="utf-8" ?> 
<job  name="D${MYJOBNAME}_R${RUNNUMBER}_" maxFilesPerProcess="$NMAXPROCESSFILES" fileListSyntax="xrootd" simulateSubmission="$SIMULATE" >

	<input URL="catalog:star.bnl.gov?production=$PROD,\
library=$LIB,\
trgsetupname=$TRGSETUP,\
filetype=daq_reco_picoDst,sname2=st_physics,\
runnumber=$RUNNUMBER,\
storage!=hpss" nFiles="$NFILES" />

	<stdout URL="file:$OUTDIR/$SYSTEM/$MYJOBNAME/log/${RUNNUMBER}_\$JOBINDEX.log" />
	<stderr URL="file:$OUTDIR/$SYSTEM/$MYJOBNAME/log/${RUNNUMBER}_\$JOBINDEX.err" />
	<output fromScratch="${RUNNUMBER}_\$JOBINDEX.tree.root" toURL="file:$OUTDIR/$SYSTEM/$MYJOBNAME/Events/" /> 
	<output fromScratch="${RUNNUMBER}_\$JOBINDEX.hist.root" toURL="file:$OUTDIR/$SYSTEM/$MYJOBNAME/Histograms/" /> 
	<output fromScratch="${RUNNUMBER}_\$JOBINDEX.mixed.root" toURL="file:$OUTDIR/$SYSTEM/$MYJOBNAME/MixedEvents/" />
	<Generator> 
		<Location>$OUTDIR/$SYSTEM/$MYJOBNAME/gen/</Location> 
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

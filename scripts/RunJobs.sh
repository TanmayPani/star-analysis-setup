#!/bin/sh

JOBUSERNAME=$1

echo "###############################################################################################"
echo "running kinit: "
eval kinit $USER
echo "running aklog: "
eval aklog
echo "###############################################################################################"

OUTDIR="/gpfs01/star/pwg/tpani/output/PicosToTree"
FASTJETINSTALL="/star/u/tpani/FastJet/fastjet-install"

mkdir -p $OUTDIR
mkdir -p $OUTDIR/$JOBUSERNAME
mkdir -p $OUTDIR/$JOBUSERNAME/gen
mkdir -p $OUTDIR/$JOBUSERNAME/log
mkdir -p $OUTDIR/$JOBUSERNAME/out 
mkdir -p $OUTDIR/$JOBUSERNAME/out/EventTrees
mkdir -p $OUTDIR/$JOBUSERNAME/out/JetTrees 

MYJOBNAME=$(date +%Y-%m-%d)
NMAXPROCESSFILES="10"
SIMULATE="false"

PROD="P18ih"
LIB="SL20d"
TRGSETUP="AuAu_200_production_mid_2014"
RUNSTART="15076101"
RUNEND="15167014"
RUNRANGE="$RUNSTART-$RUNEND"

if [[ $SIMULATE == "true" ]]; then
	NFILES="20"
elif [[ $SIMULATE == "false" ]]; then
	NFILES="all"
else
	NFILES="all"
fi

XMLSCRIPT="JOBXML_FILES/SubmitJobs_$JOBUSERNAME.xml"

cat> "$XMLSCRIPT" <<EOL
<?xml version="1.0" encoding="utf-8" ?> 
<job  name="$MYJOBNAME" maxFilesPerProcess="$NMAXPROCESSFILES" fileListSyntax="xrootd" simulateSubmission="$SIMULATE" >

	<input URL="catalog:star.bnl.gov?production=$PROD,library=$LIB,trgsetupname=$TRGSETUP,filetype=daq_reco_picoDst,sname2=st_physics,runnumber[]$RUNRANGE,storage!=hpss" nFiles="$NFILES" />	
	<stdout URL="file:$OUTDIR/$JOBUSERNAME/log/\$JOBID.log" />
	<stderr URL="file:$OUTDIR/$JOBUSERNAME/log/\$JOBID.err" />
	<output fromScratch="EventTree_\$JOBINDEX.root" toURL="file:$OUTDIR/$JOBUSERNAME/out/EventTrees/" /> 
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
		ln -s $FASTJETINSTALL/include/fastjet
		ln -s $FASTJETINSTALL/include/siscone

		starver new
		setenv FASTJET $FASTJETINSTALL

	    root -b -q -l readPicoDstAnalysisMaker.C\(\"\$FILELIST\",\"\$JOBINDEX.root\",1000000000\)
	   
	    unlink fastjet
	    unlink siscone   
	</command>

</job>

EOL

star-submit $XMLSCRIPT

#bash concheck.sh $myDate $BNLusername

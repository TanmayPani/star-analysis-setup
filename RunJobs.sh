#!/bin/sh

JOBUSERNAME=$1

BNLuser="tpani" 
echo "###############################################################################################"
echo "###############################################################################################"
echo "trying with eval command:"
echo "running kinit: "
eval kinit $BNLuserName
echo "running aklog: "
eval aklog
echo "###############################################################################################"

myScratch="/gpfs01/star/pwg/tpani/output"

mkdir $myScratch/PicosToTree/$JOBUSERNAME
mkdir $myScratch/PicosToTree/$JOBUSERNAME/gen
mkdir $myScratch/PicosToTree/$JOBUSERNAME/log
mkdir $myScratch/PicosToTree/$JOBUSERNAME/out 
mkdir $myScratch/PicosToTree/$JOBUSERNAME/out/EventTrees
mkdir $myScratch/PicosToTree/$JOBUSERNAME/out/JetTrees 

XMLSCRIPT="SubmitJobs_$JOBUSERNAME.xml"

cat> "$XMLSCRIPT" <<EOL
<?xml version="1.0" encoding="utf-8" ?> 
<job  name="myPicoJobs" maxFilesPerProcess="10" fileListSyntax="xrootd" simulateSubmission ="false" >

	<input URL="catalog:star.bnl.gov?production=P18ih,library=SL20d,trgsetupname=AuAu_200_production_mid_2014,filetype=daq_reco_picoDst,sname2=st_physics,runnumber[]15076101-15167014,storage!=hpss" nFiles="all" />	
	<stdout URL="file:/gpfs01/star/pwg/tpani/output/PicosToTree/$JOBUSERNAME/log/\$JOBID.log" />
	<stderr URL="file:/gpfs01/star/pwg/tpani/output/PicosToTree/$JOBUSERNAME/log/\$JOBID.err" />
	<output fromScratch="EventTree_\$JOBINDEX.root" toURL="file:/gpfs01/star/pwg/tpani/output/PicosToTree/$JOBUSERNAME/out/EventTrees/" /> 

	<Generator> 
		<Location>/gpfs01/star/pwg/tpani/output/PicosToTree/$JOBUSERNAME/gen/</Location> 
	</Generator>

    <SandBox>
        <Package>
            <File>file:./readPicoDstAnalysisMaker.C</File>
            <File>file:./StRoot/</File>
            <File>file:./.sl73_gcc485/</File>
        </Package>
    </SandBox>

	<command>
		ln -s /star/u/tpani/FastJet/fastjet-install/include/fastjet
		ln -s /star/u/tpani/FastJet/fastjet-install/include/siscone

		starver new
		setenv FASTJET /star/u/tpani/FastJet/fastjet-install

	    root -b -q -l readPicoDstAnalysisMaker.C\(\"\$FILELIST\",\"\$JOBINDEX.root\",1000000000\)
	   
	    unlink fastjet
	    unlink siscone   
	</command>

</job>

EOL

star-submit SubmitJobs_$JOBUSERNAME.xml

#bash concheck.sh $myDate $BNLusername

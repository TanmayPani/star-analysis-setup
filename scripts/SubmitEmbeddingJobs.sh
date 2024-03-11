#!/bin/sh
echo "###############################################################################################"
echo "running kinit: "
eval kinit $USER
echo "running aklog: "
eval aklog
echo "###############################################################################################"
#CURRENTDIR=$(pwd)
DATE=$(date +%Y%m%d)
#DATE="20230806"

MYJOBNAME="MuToTree$DATE"
NFILESPERJOB=10
SIMULATE="false"

PROD="P12id"
LIB="SL12d"
YEAR="2012"
TRGSETUP="pp200_production_2012"
EMBEDDINGDATE="20212001"
FILETYPE="MuDst.root"

OUTFOLDERNAME="Pythia6Embedding_${TRGSETUP}_${PROD}_${LIB}_${EMBEDDINGDATE}_${MYJOBNAME}"
OUTDIR="/gpfs01/star/pwg/$USER/output/$OUTFOLDERNAME"
XMLDIR="jobSubmitScripts/$OUTFOLDERNAME"
FILELISTDIR="fileLists/$OUTFOLDERNAME"

#rm -rf $OUTDIR
#rm -rf $XMLDIR
#rm -rf $FILELISTSDIR
mkdir -p $OUTDIR
mkdir -p jobSubmitScripts
mkdir -p $XMLDIR
mkdir -p fileLists 
mkdir -p $FILELISTDIR

#ptHats=("45_55" "35_45" "25_35" "20_25" "15_20" "11_15")
ptHats=("55_-1" "45_55" "35_45" "25_35" "20_25" "15_20" "11_15")

EMBEDDINGHEAD="/star/embed/embedding/$TRGSETUP/v2/Pythia6_"
nGroups=0
#ITERATION=0
for pth in "${ptHats[@]}"; do
    echo "##################### Working on ptHat: $pth #####################"
    nGroups=0
    fileDir="${EMBEDDINGHEAD}pt${pth}"
    for folder in $fileDir*; do
        #ITERATION=$((ITERATION+1))
        echo "###############################################################################################"
        echo "Working on folder: $folder"
        fileListName=$FILELISTDIR/pt${pth}_${nGroups}.list
        echo "Creating file list: $fileListName"
        echo "Finding files of type: $FILETYPE"
        find "$folder/" -type f -name "*$FILETYPE" > $fileListName

        finalOutDir="${OUTDIR}/pt${pth}_${nGroups}"
        mkdir -p $finalOutDir
        mkdir -p $finalOutDir/tree
        mkdir -p $finalOutDir/hist 
        mkdir -p $finalOutDir/log
        mkdir -p $finalOutDir/gen
        echo "Output folder: $finalOutDir"
        echo "Creating XML job file for list: $fileListName"

        XMLFILE="$XMLDIR/${pth}_${nGroups}.xml"

cat> "$XMLFILE" <<EOL
<?xml version="1.0" encoding="utf-8" ?>
<job  name="${MYJOBNAME}_${pth}_${nGroups}" maxFilesPerProcess="$NFILESPERJOB" fileListSyntax="paths" simulateSubmission ="$SIMULATE" >

    <input URL="filelist:$(pwd)/$fileListName" nFiles="all" />
    <stdout URL="file:$finalOutDir/log/\$JOBID.log" />
    <stderr URL="file:$finalOutDir/log/\$JOBID.err" />
    <output fromScratch="*.tree.root" toURL="file:$finalOutDir/tree/" /> 
    <output fromScratch="*.hist.root" toURL="file:$finalOutDir/hist/" /> 
    <Generator> 
        <Location>$finalOutDir/gen/</Location>
    </Generator> 

    <SandBox installer="ZIP">
        <Package name="${MYJOBNAME}">
            <File>file:./genDst.C</File>
            <File>file:./readPicoDstAnalysisMaker.C</File>
            <File>file:./additionalFiles/</File>
            <File>file:./StRoot/</File>
            <File>file:./.sl73_gcc485/</File>
        </Package>
    </SandBox>

    <command>
        starver new
        ln -s $FASTJET/include/fastjet
        ln -s $FASTJET/include/siscone
        setenv FASTJET $FASTJET

        @ nFile=0
        while ( \$nFile &lt; \$INPUTFILECOUNT )
            eval set fileName='\$INPUTFILE'\${nFile}
            echo "Processing file: \$fileName"
            root4star -l -b -q 'genDst.C(-1, "'"picoDst,PicoVtxMode:PicoVtxVpdOrDefault,PicoCovMtxMode:PicoCovMtxWrite"'", "'"\$fileName"'")'
            echo "converted file: \$fileName into picoDst.root"

            root -b -q -l readPicoDstAnalysisMaker.C\(\"\$fileName\",\"dummyFileName.root\",1000000000,true\)

            rm -f ./INPUTFILES/*.picoDst.root
            @ nFile++
        end
    </command>
        
</job>
EOL
        echo "Submitting job for file list: $fileListName"
        star-submit $XMLFILE
        nGroups=$(($nGroups+1))
    done
    rm -rf *.session.xml
    rm -rf *.dataset
    echo "###############################################################################################"
done




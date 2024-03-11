# Create the file list
PROD=$1
LIB=$2
YEAR=$3
TRGSETUP=$4
PTHBIN=$5
EMBEDDINGDATE=$6

FILETYPE=".MuDst.root"
MYJOBNAME="MuToTree"

FOLDERNAME="/star/embed/embedding/$TRGSETUP/v2/Pythia6_"
FILEDIR="${FOLDERNAME}pt${PTHBIN}"

FILELISTDIR="fileLists/Pythia6Embedding_${TRGSETUP}_${PROD}_${LIB}_${EMBEDDINGDATE}_$MYJOBNAME"
mkdir -p fileLists 
mkdir -p $FILELISTDIR

nGroups=0
for folder in $FILEDIR*; do
    echo "Working on folder: $folder"
    FILELISTNAME=$FILELISTDIR/pt${PTHBIN}_${nGroups}.list
    echo "Creating file list: $FILELISTNAME"
    echo "Finding files of type: $FILETYPE"
    find "$folder/" -type f -name "*$FILETYPE" > $FILELISTNAME
    nGroups=$((nGroups+1))
done


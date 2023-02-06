#!/bin/bash


#Variables with necessary paths
SourcePath='./'
DestinationPath='/tmp/quesanumber'
SourceFile='*.c'

#Сhecking for the existence of a folder with the necessary files
if [ -d $SourcePath ]; then
    cd "$SourcePath"
    if [ ! -f $SourceFile ]; then
        echo "The files not exist"
        exit 1
    fi
else
    echo "The directory with files not exist"
    exit 2
fi
cd -
echo
#Checking for the existence of the target folder 
if [ ! -d $DestinationPath ]; then
    mkdir "$DestinationPath"
    echo "$DestinationPath was created"
fi
#Copying Source files
cp -v $SourcePath/$SourceFile "$DestinationPath"
#Archive Source files
tar -czvf "$DestinationPath/QuessNumber.tar.gzip" "$DestinationPath"
#Copying archive to release
mkdir ../release
cp "$DestinationPath/QuessNumber.tar.gzip" ../release
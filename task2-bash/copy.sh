#!/bin/bash

#defile folder names
TEMP_DIR=/tmp/guesanumber
RELEASE_DIR_NAME=release
ARJ_NAME=guesanumber.tar.gz
CURRENT_DIR=$(pwd)

#prepare folders

echo Preparing folders

if [ -d $TEMP_DIR ];
then
	rm -r  $TEMP_DIR
fi
mkdir $TEMP_DIR
if [ -d $CURRENT_DIR/../$RELEASE_DIR_NAME ];
then
        rm  -r $CURRENT_DIR/../$RELEASE_DIR_NAME
fi
mkdir $CURRENT_DIR/../$RELEASE_DIR_NAME


#copy source files to temp folder

echo Copying .c sources and compressing
cp $CURRENT_DIR/../task1-simple-program/*.c $TEMP_DIR/

#create tar archive
cd $TEMP_DIR
tar -czf $ARJ_NAME *


#copy archive to release folder
cp $ARJ_NAME $CURRENT_DIR/../$RELEASE_DIR_NAME
cd $CURRENT_DIR
echo guesanumber.tar.gz created in release folder of project root

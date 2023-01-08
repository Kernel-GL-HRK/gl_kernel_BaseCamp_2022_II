#!/bin/bash

AR_NAME=guessanumber
SRC_DIR=../task1-simple-program
TMP_DIR=/tmp/$AR_NAME
RELEASE=$(pwd)/release

#find all source files in source folder
FILES=`find $SRC_DIR -name '*.c' -or -name '*.h'`

#create folders
mkdir -p $TMP_DIR 2>/dev/null
mkdir -p $RELEASE 2>/dev/null

cp $FILES $TMP_DIR

tar -czvf $RELEASE/$AR_NAME.tar.gz $TMP_DIR

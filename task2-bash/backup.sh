#!/bin/bash

SOURCE_DIR=../task1-simple-program
DEST_DIR_NAME=guessanumber
DEST_DIR=/tmp/$DEST_DIR_NAME
RELEASE_DIR=./release

FILES=`find $SOURCE_DIR -name *.c`
echo $FILES
if [[ ! -d "$DEST_DIR" ]]
then
	mkdir -p $DEST_DIR
fi

if [[ ! -d $RELEASE_DIR ]]
then
	mkdir -p $RELEASE_DIR
fi

cp $FILES $DEST_DIR

tar -czvf $RELEASE_DIR/$DEST_DIR_NAME.gz $DEST_DIR

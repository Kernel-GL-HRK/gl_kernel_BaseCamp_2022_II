#!/bin/bash

SOURCE_DIR=../task1-simple-program
DEST_DIR_NAME=guessanumber
DEST_DIR=/tmp/$DEST_DIR_NAME
RELEASE_DIR=./release

FILES=`find $SOURCE_DIR -name '*.c' -or -name '*.h'`

mkdir -p $DEST_DIR 2> /dev/null
mkdir -p $RELEASE_DIR 2> /dev/null

cp $FILES $DEST_DIR

tar -czvf $RELEASE_DIR/$DEST_DIR_NAME.tar.gz $DEST_DIR

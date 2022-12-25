#!/bin/bash

SOURCE_DIR=../task1-simple-program
DEST_DIR=/tmp/guessanumber
RELEASE_DIR=./release

FILES=`find $SOURCE_DIR -name *.c`

if [[ ! -d "$DEST_DIR" ]]
then
	mkdir -p $DEST_DIR
fi

if [[ ! -d $RELEASE_DIR ]]
then
	mkdir -p $RELEASE_DIR
fi

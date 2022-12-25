#!/bin/bash

SOURCE_DIR=../task1-simple-program
DEST_DIR=/tmp/guessanumber

FILES=`find $SOURCE_DIR -name *.c`

if [[ ! -d "$DEST_DIR" ]]
then
	mkdir -p $DEST_DIR
fi

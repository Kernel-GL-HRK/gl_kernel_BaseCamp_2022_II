#!/bin/bash

SRC_DIR=../task1-simple-program
TMP_DIR=/tmp/guessanumber

#find all source files in source folder
FILES=`find $SOURCE_DIR -name '*.c' -or -name '*.h'`

#create folders
mkdir -p $TMP_DIR 2>/dev/null
mkdir -p $(pwd)/release 2>/dev/null

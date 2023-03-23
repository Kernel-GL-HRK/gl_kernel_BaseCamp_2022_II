#!/bin/bash

SRC_CODE="../task1-simple-program/guess_a_number.c"
PROGRAM="guessanumber"
DEST_PATH="/tmp/$PROGRAM"
ARCHIVE="$PROGRAM.tar.gz"

#if [[ -d "$DEST_PATH" ]]; then
#    rm -rf "$DEST_PATH"
#fi
mkdir "$DEST_PATH"

#copping all source code of your project form part1 to /tmp/guesanumber.
cp "$SRC_CODE" "$DEST_PATH"

#if [[ -f "$ARCHIVE" ]]; then
#    rm -f "$ARCHIVE"
#fi

#Compress this folder to gzip archive (please google tar command) with same name.
tar -zcf "$ARCHIVE" --absolute-names "$DEST_PATH/"

DEST_PATH="release"
#if [[ -d "$DEST_PATH" ]]; then
#    rm -rf "$DEST_PATH"
#fi
mkdir "$DEST_PATH"

#Copy gzip archive to “release” subdirectory in project dir.
cp "$PROGRAM.tar.gz" "$DEST_PATH"

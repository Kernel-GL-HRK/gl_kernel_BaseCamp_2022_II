#!/bin/bash

# script is copying files from the first argument to /tmp/guessanumber
# compressing the folder
# copy gzip to the release subdirectory

PATH_TO_SOURCES=$1
TARGET_PATH=$2

set -e

# copy files
cp -r $PATH_TO_SOURCES $TARGET_PATH

# tar the target directory
tar -czvf guessanumber.tar.gz $TARGET_PATH

# create release directory
mkdir -p release/

# copy tar to release/
cp guessanumber.tar.gz release/

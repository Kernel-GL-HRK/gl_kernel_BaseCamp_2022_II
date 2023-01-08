#!/bin/bash

PROJECT_PATH=../task1-simple-program
DIR_PATH=/tmp/guesanumber

if [ ! -d "$DIR_PATH" ]; then
    mkdir $DIR_PATH
fi

cp $PROJECT_PATH/*.c $DIR_PATH

tar cvzf /tmp/guesanumber.tar.gz -C /tmp guesanumber 
#!/bin/bash                                                                                                                    

PART1_PATH=../task1-simple-program/
FOLDER=guesanumber
COPY_PATH=/tmp/$FOLDER

mkdir -p $COPY_PATH

cp $PART1_PATH/*.[ch] $COPY_PATH
                                                                                   
ARCHIVE=$FOLDER.tar.gzip

cd $COPY_PATH/.. && tar -czf $ARCHIVE $FOLDER
cd - > /dev/null

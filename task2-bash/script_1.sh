#!/bin/bash
# script_1.sh

path_destination=~/tmp/guesanumber

if [ -d $path_destination ]
then
        echo "$path_destination - directory already exists"
else
        mkdir -p $path_destination
fi

cd $path_destination

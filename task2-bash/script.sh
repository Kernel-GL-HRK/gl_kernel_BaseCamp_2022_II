#!/bin/bash

source_dir="/tmp/guesanumber"

#check if "/tmp/guesanumber" exist, if not - create
if [[ ! -d $source_dir ]]; then
	mkdir $source_dir
fi

cp ../task1-simple-program/*.c  -t /tmp/guesanumber/
# cp ../task1-simple-program/*.c ../task1-simple-program/*.h -t /tmp/guesanumber/

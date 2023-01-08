#!/bin/bash
# script_1.sh

path_source=~/gl_base_camp/gl_kernel_BaseCamp_2022_II/task1-simple-program
path_destination=/tmp/guesanumber

if [ -d $path_destination ]
then
        echo "$path_destination - directory already exists"
else
        mkdir -p $path_destination
fi

cd $path_destination
cp $path_source/* ./

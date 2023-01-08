#!/bin/bash
# script_1.sh

path_source=~/gl_base_camp/gl_kernel_BaseCamp_2022_II/task1-simple-program
path_destination=/tmp/guesanumber
subdir=release

if [ -d $path_destination ]
then
        echo "$path_destination - directory already exists"
else
        mkdir -p $path_destination
fi

cd $path_destination
cp $path_source/* ./

destination_name=${PWD##*/}
tar -czf $destination_name.tar.gzip ./*

mkdir -p $path_source/${subdir}
cp $path_destination/$destination_name.tar.gzip $path_source/${subdir}

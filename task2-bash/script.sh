#!/bin/bash

source_dir="/tmp/guesanumber"

#check if source_dir  exist, if not - create
if [[ ! -d $source_dir ]]; then
	mkdir -p $source_dir
fi

cp ../task1-simple-program/*.c  -t $source_dir
# cp ../task1-simple-program/*.c ../task1-simple-program/*.h -t $source_dir


#cd to source_dir is done to prevent saving full path in archives
cd $source_dir
tar -cvf "../$(basename $source_dir).tar"  "../$(basename $source_dir)"
tar -czvf "../$(basename $source_dir).tar.gz"  "../$(basename $source_dir)" 
cd $OLDPWD


r_dir="../release"

if [[ ! -d $r_dir ]]; then
        mkdir -p $r_dir
fi

cp "${source_dir}.tar.gz" -t $r_dir

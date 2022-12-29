#!/bin/bash -ex

src_folder="../task1-simple-program"
tmp_folder="/tmp/guesanumber"
release_folder="$src_folder/release"


## Flushing tmp directory
[ -d $tmp_folder ] && rm -f $tmp_folder/*

## Copying files to tmp folder
mkdir -p $tmp_folder
cp $(find $src_folder -type f -name "*.[c,h]") $tmp_folder

## Archiving backup to release folder
mkdir -p $release_folder
tar --directory=$(dirname $tmp_folder) -cvzf \
	$release_folder/$(basename $tmp_folder).tar.gz \
	$(basename $tmp_folder)

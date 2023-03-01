#!/bin/bash -ex

src="../task1-simple-program"
tmp="/tmp/guesanumber"
release="$src/release"


## Flushing tmp directory
[ -d $tmp ] && rm -f $tmp/*

## Copying files to tmp directory
mkdir -p $tmp
cp $(find $src -type f -name "*.[c,h]") $tmp

## Archiving backup to release directory
mkdir -p $release
tar --directory=$(dirname $tmp) -cvzf \
	$release/$(basename $tmp).$(date "+%F.%H-%M-%S").tar.gz \
	$(basename $tmp)

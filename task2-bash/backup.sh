#!/bin/bash -ex

src_dir="../task1-simple-program"
tmp_dir="/tmp/guesanumber"
release_dir="${src_dir}/release"


## Flushing tmp directory
[ -d "$tmp_dir" ] && rm -f "${tmp_dir}/*"

## Copying files to tmp directory
mkdir -p "$tmp_dir"
cp $(find ${src_dir} -type f -name "*.[c,h]") "$tmp_dir"

## Archiving backup to release directory
mkdir -p "$release_dir"
tar --directory=$(dirname "${tmp_dir}") -cvzf \
	"${release_dir}/$(basename ${tmp_dir}).$(date "+%F.%H-%M-%S").tar.gz" \
	"$(basename ${tmp_dir})"

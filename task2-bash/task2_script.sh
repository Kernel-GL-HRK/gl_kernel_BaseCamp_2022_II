#!/bin/bash

src_dir=../task1-simple-program
dest_dir=/tmp/guesanumber

create_or_check_dir(){
    if [[ ! -e $1 ]]; then
        mkdir $1
    elif [[ ! -d $1 ]]; then
        echo "$1 already exists but is not a directory" 1>&2
    fi
}

create_or_check_dir "$dest_dir"

#cp -R "$src_dir" "$dest_dir"
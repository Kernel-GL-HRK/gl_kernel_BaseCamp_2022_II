#!/bin/bash
RED='\033[0;31m'
Blue='\033[0;34m'
NC='\033[0m'

src_dir=../task1-simple-program
dest_dir=/tmp/guesanumber
ar_name=guesanumber
release_dir=./release

create_or_check_dir(){
    if [[ ! -e $1 ]]; then
        mkdir $1
        echo "${BLUE}Created $1 folder${NC}"
    elif [[ ! -d $1 ]]; then
        echo "${RED}$1 already exists but is not a directory${NC}" 1>&2
    fi
}

echo -e "${BLUE}Checking destination folder - $dest_dir ${NC}"
create_or_check_dir "$dest_dir"

cp -R $src_dir/src "$dest_dir"

if [[ ! -e "$dest_dir"/src ]]; then
    echo "Destination folder doesnt contain Src folder"
else
    echo "${BLUE}Making archive${NC}"
    tar -zcvf "$ar_name".tar.gz "$dest_dir"/src
fi

echo "${BLUE}Checking release folder${NC}"
create_or_check_dir "$release_dir"

echo "${BLUE}Copying archive to release folder${NC}"
cp -R "$ar_name".tar.gz "$release_dir"

#! /bin/bash

BLUE='\033[0;34m'
BYELLOW='\033[1;33m'
BGREEN='\033[1;32m'
YELLOW='\033[0;33m'
GREEN='\033[0;32m'
BWHITE='\033[1;37m' 
UWHITE='\033[4;37m'
RED='\033[0;31m' 
BRED='\033[1;31m'  
NC='\033[0m' # No Color

source=../task1-simple-program
dest=./tmp/guesanumber
dest2=../task1-simple-program/release
arch=../task1-simple-program/release/task1-simple-program.tar

echo -e "\n${BWHITE}${UWHITE}BACKUP FILE${NC}"
echo -e "Copy from ${BGREEN}$source/*.* ${NC}${UWHITE}only${NC} ${UWHITE}files${NC} without ${UWHITE}git${NC} ${UWHITE}files${NC} and ${UWHITE}subdirectories${NC}"
echo -e "To the directory ${BYELLOW}${dest}/*.* ${NC} \nand archive to a subdirectory ${BYELLOW}\"${source}/release/*.gz\"${NC}\n"

if [ ! -d ${dest} ];
then
    mkdir -p ${dest}
fi
if [ ! -d ${dest2} ];
then
    mkdir -p ${dest2} 
fi

for file in $(find $source -maxdepth 1 -type f -not -name ".git*" -printf "%P\n") ; do 
    if [ -a $dest/$file ] ; 
    then
        if [ $source/$file -nt $dest/$file ] ; then
        echo -e "Newer ${YELLOW}$fil  e${NC} detected, copying..."
        cp $source/$file $dest/$file  
        else
        echo -e "File ${BLUE}$file${NC} exists, skipping."
        fi
    else
        echo -e "File ${GREEN}$file${NC} is being copied over to ${BYELLOW}$dest/${NC}"
        cp $source/$file $dest/$file
    fi

    if [ -f $arch.gz ] ; then
        gunzip $arch.gz
        tar --update -vPf $arch $source/$file > /dev/null 
    else
        tar --update -vPf $arch $source/$file > /dev/null 
    fi
 done
for file in $(find $dest -maxdepth 1 -printf "%P\n") ; do 
    if [ ! -f $source/$file ] ; then
        echo -e "Delete ${RED}$file${NC} file, do not panic the file is in the archive ${BWHITE}\"${arch}\"${NC}. May be."
        rm -f $dest/$file  
    fi
done

echo -e "\n${BYELLOW}********** ${arch}.gz  ***********${NC}"
tar -tvPf $arch
gzip -f $arch # -f --force overwrite existing .tar.gz
echo -e "${BYELLOW}***********************************************************************************${NC}"

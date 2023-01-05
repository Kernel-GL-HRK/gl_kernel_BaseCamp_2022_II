#! /bin/bash

BLUE='\033[0;34m'
BYELLOW='\033[1;33m'
BGREEN='\033[1;32m'
BWHITE='\033[1;37m' 
UWHITE='\033[4;37m'
RED='\033[0;31m'  
NC='\033[0m' # No Color

source=../task1-simple-program
dest=./tmp/guesanumber
arch=../task1-simple-program/release/task1-simple-program.tar

echo -e "\n${BWHITE}${UWHITE}BACKUP FILE${NC}"
echo -e "Copy from ${BGREEN}$source/*.* ${NC}${UWHITE}only${NC} ${UWHITE}files${NC} without ${UWHITE}git${NC} ${UWHITE}files${NC} and ${UWHITE}subdirectories${NC}"
echo -e "To the directory ${BYELLOW}${dest}/*.* ${NC} \nand archive to a subdirectory ${BYELLOW}\"${source}/release/*.gz\"${NC}\n"
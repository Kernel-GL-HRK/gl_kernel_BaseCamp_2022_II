#!/bin/bash

Color_Off='\033[0m' # Text Reset
Red='\033[0;31m'    # Red
On_White='\033[47m' # White

echo -e "${Red}${On_White}To archive source code${Color_Off}"
echo -e "${Red}${On_White}run this script from the project directory${Color_Off}"

# Copy source code
rm -rf /tmp/guesanumber 2>/dev/null
mkdir /tmp/guesanumber
cp --archive ./*.c ./*.h ./*.md ./*.sh /tmp/guesanumber 2>/dev/null

#Compress the directory
tar -zcvf guesanumber.tar.gz /tmp/guesanumber

#Copy the archive to project subdirectory
mkdir ./release 2>/dev/null
cp -f ./guesanumber.tar.gz ./release

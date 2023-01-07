#!/bin/bash
RED='\033[0;31m'
Blue='\033[0;34m'
NC='\033[0m'

src_dir=../task1-simple-program/

if [ ! -e $src_dir/a.out ]; then
    echo "${RED}Exutable doenst exist${NC}"
fi

help() {
    echo "Press:"
    echo "y - to play"
    echo "n - to leave"
    echo "or number you bet on"
}

while true;
do
    help
    
    $src_dir/a.out     
done
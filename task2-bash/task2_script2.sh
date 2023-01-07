#!/bin/bash
RED='\033[0;31m'
Blue='\033[0;34m'
NC='\033[0m'

game=../task1-simple-program/a.out

win_counter=0

#if [ ! -e $src_dir/a.out ]; then
#    echo "${RED}Exutable doenst exist${NC}"
#fi

help() {
    echo "Press:"
    echo "y - to play"
    echo "n - to leave"
    echo "or number you bet on"
}

start_game() {
    if [ "$number" -ge 0 ] && [ "$number" -le 9 ]; then
        $game "$number"
    fi
}

while true;
do
    help
    read -r input

    if [[ $input = "y" ]]; then
        echo "I need a number"
        read -r number
        start_game "$number"
    elif [[ $input = "n" ]]; then
        echo "You are leaving with $win_counter wins"
        break
    else 
        start_game "$input" 
    fi
   
done
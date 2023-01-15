#!/bin/bash

# Path to game "guess a number":
GAME="../task1-simple-program/guess_a_number.out"

# Colors:
red="\e[1;31m"
green="\e[1;32m"
yellow="\e[1;33m"
reset="\e[m"

# Variables:
attempt_cnt=0
win_cnt=0

# Command to enable extended pattern matching:
shopt -s extglob

while true; do

    if [ $attempt_cnt -eq 0 ]; then
        echo -e "${yellow}Enter Y to continue, N to exit, or the number of${reset}"
        echo -e "${yellow}attempts you want to play without interruption:${reset}"
        read reply
        case $reply in
            Y|y)        ;;
            N|n)        break ;;
            +([0-9]))   let attempt_cnt=reply-1 ;;
            *)          echo -e "${red}Invalid choice!${reset}"
                        continue ;;
        esac
    else 
        let attempt_cnt--
    fi

    $GAME

    if [ $? -eq 0 ]; then
        let win_cnt++
        echo -e "${green}Good job!${reset}"
    else
        echo -e "${red}Wish a good luck next time.${reset}"
    fi

    echo -e "${yellow}You have won $win_cnt time(s).${reset}"
    
done

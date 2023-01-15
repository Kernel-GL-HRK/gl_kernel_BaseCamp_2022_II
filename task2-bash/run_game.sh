#!/bin/bash

# Path to game "guess a number":
GAME="../task1-simple-program/guess_a_number.out"

# Variables:
attempt_cnt=0
win_cnt=0

# Command to enable extended pattern matching:
shopt -s extglob

while true; do

    if [ $attempt_cnt -eq 0 ]; then
        echo "Enter Y to continue, N to exit, or the number of"
        echo "attempts you want to play without interruption:"
        read reply
        case $reply in
            Y|y)        ;;
            N|n)        break ;;
            +([0-9]))   let attempt_cnt=reply-1 ;;
            *)          echo "Invalid choice!"
                        continue ;;
        esac
    else 
        let attempt_cnt--
    fi

    $GAME

    if [ $? -eq 0 ]; then
        let win_cnt++
        echo "Good job!"
    else
        echo "Wish a good luck next time."
    fi

    echo "You have won $win_cnt time(s)."
    
done

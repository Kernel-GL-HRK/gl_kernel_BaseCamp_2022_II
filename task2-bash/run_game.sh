#!/bin/bash

# Path to game "guess a number":
GAME="../task1-simple-program/guess_a_number.out"

while true; do

    echo "Enter Y to continue or N to exit:"
    read reply
    case $reply in
        Y|y)    ;;
        N|n)    break ;;
        *)      echo "Invalid choice!"
                continue ;;
    esac

    $GAME
    
done

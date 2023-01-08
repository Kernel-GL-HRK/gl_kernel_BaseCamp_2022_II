#!/usr/bin/env bash

score=0
function score_update ()
{
    if [[ $? == 0 ]]; then
        score=$((score+1))
        echo "Good job"
    else
        echo "Wish a good luck next time"
    fi
}

# Program name
PROG_NAME='guess'

# Compile the simple program
gcc ../task1-simple-program/"$PROG_NAME".c -o "$PROG_NAME"

# Shell option that allows the shell to recognize various extended pattern matching operators
shopt -s extglob


while [ true ]
do
	echo "Press 'y' to continue, 'n' to exit. Or press number to set number of runs of tries."
	read Variable
    case "$Variable" in
        y        ) ./"$PROG_NAME"; score_update;;
        n        ) break;;
        +([0-9]) ) echo "Loop"; break;;
        *        ) echo "Sorry, please try one more time."
    esac
    echo "$score"
done

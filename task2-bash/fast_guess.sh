#!/usr/bin/env bash

# Shell option that allows the shell to recognize various extended pattern matching operators
shopt -s extglob

# Program name and initialize score
PROG_NAME='guess'
score=0

# Compile the simple program
gcc -w ../task1-simple-program/"$PROG_NAME".c -o "$PROG_NAME"

# Calculating points and printing messages
function score_update ()
{
    if [[ $? == 0 ]]; then
        score=$((score+1))
        echo "Good job!"
    else
        echo "Wish good luck next time"
    fi
    echo "Number of success case(s): $score"
    echo "======="
}

# Iterate over the number of trials
function loop ()
{
    for ((c=1; c<="$1"; c++))
    do 
        ./"$PROG_NAME"
        score_update
    done
}

# Ask the user to guess
while [ true ]
do
	echo "Press 'y' to continue, 'n' to exit. Or press number to set number of runs of tries."
	read Variable
    case "$Variable" in
        y | Y    ) ./"$PROG_NAME"; score_update;;
        n | N    ) break;;
        +([0-9]) ) loop Variable; break;;
        *        ) echo "Sorry, please try one more time.";;
    esac
done

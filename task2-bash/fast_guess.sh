#!/usr/bin/env bash

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
        y        ) ./"$PROG_NAME";;
        n        ) break;;
        +([0-9]) ) echo "Loop"; break;;
    esac
done
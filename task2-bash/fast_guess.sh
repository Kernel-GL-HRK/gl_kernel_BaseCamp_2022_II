#!/usr/bin/env bash

# Program name
PROG_NAME='guess'

# Compile the simple program
gcc ../task1-simple-program/"$PROG_NAME".c -o "$PROG_NAME"

# Test
./"$PROG_NAME"
#!/usr/bin/env bash

# Temporary directory name and project directory path
TEMP_NAME='guesanumber'
PROJ_DIR='../task1-simple-program'

# Copy all source code of my project form part 1
mkdir -p /tmp/"$TEMP_NAME"/
cp "$PROJ_DIR"/*.[ch] /tmp/"$TEMP_NAME"/

# Compress temporary folder to gzip archive with same name
tar -czvf "$TEMP_NAME".tar.gz /tmp/"$TEMP_NAME"

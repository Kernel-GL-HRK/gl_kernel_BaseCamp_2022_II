#!/usr/bin/env bash

# Temporary and sub- directory name and project directory path 
PROJ_DIR='../task1-simple-program'
TEMP_NAME='guesanumber'
SDIR_NAME='release'

# Copy all source code of my project form part 1
mkdir -p /tmp/"$TEMP_NAME"/
cp "$PROJ_DIR"/*.[ch] /tmp/"$TEMP_NAME"/

# Compress temporary folder to gzip archive with same name
tar -czvf "$TEMP_NAME".tar.gz /tmp/"$TEMP_NAME"

# Copy the gzip archive to the "release" subdirectory of the project directory
mkdir "$SDIR_NAME"
cp "$TEMP_NAME".tar.gz "$SDIR_NAME"
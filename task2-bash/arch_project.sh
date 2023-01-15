#!/bin/bash

# Parameters:
# $1 is the path to the project directory
# $2 is the name of the target archive

DEFAULT_DIRECTORY="../task1-simple-program"
DEFAULT_ARCH_NAME="guesanumber"

if [ $# -lt 1 ]; then
    echo "Warning: Arguments are not specified. Using default values."
    set $DEFAULT_DIRECTORY $DEFAULT_ARCH_NAME
fi

if [ $# -lt 2 ]; then
    echo "Warning: Archive name is not specified. Using default value."
    set $1 $DEFAULT_ARCH_NAME
fi

if [ ! -d $1 ]; then
    echo "Error: $1 is not a directory."
    exit 1
fi

echo "Copying source files from $1 to /tmp/$2..."
[ -d /tmp/$2 ] || mkdir /tmp/$2
cp $1/*.c $1/*.h /tmp/$2/

echo "Making archive /tmp/$2.tgz..."
tar czf /tmp/$2.tgz -C /tmp $2

echo "Copying archive /tmp/$2.tgz to $1/release..."
[ -d $1/release ] || mkdir $1/release
cp /tmp/$2.tgz $1/release/

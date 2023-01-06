#!/bin/sh

DIR="/tmp/guesanumber"
DEST="./release"

if [ ! -d "$DIR" ]; then
    # If directory dosen't exist create it.
    mkdir "$DIR"
fi

# If *.c files exist then copy them all.
if [ -f *.c ]; then
    cp *.c "$DIR"
fi
# If *.h files exist then copy them all.
if [ -f *.h ]; then
    cp *.h "$DIR"
fi
# Create a gzip  archive
tar -zcvf ""$DIR".tar.gz" "$DIR"

if [ ! -d "$DEST" ]; then
    # If directory dosen't exist create it.
    mkdir "$DEST"
fi
# Copy archive to the ./release
cp ""$DIR".tar.gz" "$DEST"



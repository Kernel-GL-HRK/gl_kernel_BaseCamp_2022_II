#!/bin/sh

DIR="/tmp/guesanumber"

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

tar -zcvf ""$DIR".tar.gz" "$DIR"
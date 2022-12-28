#!/bin/bash

EXEC_PATH=../task1-simple-program
EXEC_FILENAME=guessanumber

success=0
ask=""
repeats=1

if [[ ! -f "$EXEC_PATH/$EXEC_FILENAME" ]]
then
	echo "$EXEC_FILENAME doesn't exist" >&2
	exit -1
fi

if [[ ! -x "$EXEC_PATH/$EXEC_FILENAME" ]]
then
	echo "Doesn't have execution permissions" >&2
	exit -1
fi

while [[ "$ask" != n ]]
do
	echo ")type y to run program once"
	echo ")type number to run program number times without a prompt"
	echo ")type n to exit"
	read ask
done

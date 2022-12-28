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


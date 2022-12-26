#!/bin/bash

EXEC_PATH=../task1-simple-program
EXEC_FILENAME=a.out

#Counter of number successful attempts
success=0

#User input variable
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


#!/bin/bash

CURRENT_DIR=$(pwd)
EXEC_DIR=/task1-simple-program
EXEC_FILENAME=guessanumber
counts=0

execute() {
for ((i=0; i<$1; i++))
    do
	$CURRENT_DIR/..$EXEC_DIR/$EXEC_FILENAME
	result=$?

	if [[ $result -eq 0 ]]; then
	    let counts=counts+1
	    echo "Good job"
	else
	    echo "Wish a good luck next time"
	fi
done
}

echo $CURRENT_DIR/..$EXEC_DIR/$EXEC_FILENAME
if [[ ! -f "$CURRENT_DIR/..$EXEC_DIR/$EXEC_FILENAME" ]]
then
	echo "$EXEC_FILENAME doesn't exist. Please run Makefile" >&2
	exit -1
fi

while true
do
	echo press \"y\" to continue, \"n\" to exit, or number to set number of runs of tries without asking \"y or n\".

	read -p "Make your choise: " user_input

	if [[ "$user_input" =~ ^[0-9]+$ ]]; then
		execute user_input;
	elif [[ "$user_input" =~ [Nn] ]]; then
		break
	elif [[ "$user_input" =~ [Yy] ]]; then
		execute 1
	else
		echo "Wrong choise"
	fi
	echo Your luck $counts times. '\n'
	counts=0
done



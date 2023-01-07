#!/bin/bash

GAME="../task1-simple-program/game"
WINS=0

new_game(){
	$GAME
	if [ $? -eq 0 ]
	then
		echo "Good job"
		WINS=$((WINS+1))
	else
		echo "Wish a good luck next time"
	fi
}


while true; do
	echo "Press:"
	echo "'y'      - to continue"
	echo "'number' - to play without asking 'y' or 'n'"
	echo "'n'      - to exit the game. Print how many times you win"
	read INPUT
	if [ $INPUT = "y" ]; then
		new_game
	elif [[ $INPUT =~ [0-9] ]]; then
		while [ $INPUT -gt 0 ]; do
			new_game
			INPUT=$((INPUT-1))
		done
	elif [ $INPUT = "n" ]; then
		echo "Youre score: " $WINS
		break
	fi
done

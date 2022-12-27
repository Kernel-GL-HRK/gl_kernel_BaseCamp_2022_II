#!/usr/bin/bash
game="../task1-simple-program/game.out"

call_game(){
	$game
	if  [[ $? -eq 0 ]]; then
		echo "Good job"
	else
		echo "Wish a good luck next time"
	fi
}

while true; do
	read -p "Wanna play a game?:  " choice
	if [[ ${choice} = 'y' || ${choice} = 'Y' ]]; then
		call_game
	elif [[ ${choice} = 'n' || ${choice} = 'N' ]]; then
		exit 0;
	elif [[ ${choice} =~ ^[0-9]+$ ]]; then
		while [[ ${choice} -gt 0 ]]; do
			call_game
			choice=$((choice-1))
		done
	fi
done

#!/usr/bin/bash

RED="\e[31m"
GREEN="\e[32m"
ENDCOLOR="\e[0m"

wins=0
losses=0
game="../task1-simple-program/game.out"

call_game(){
	$game
	if  [[ $? -eq 0 ]]; then
		echo -e "${GREEN}Good job${ENDCOLOR}"
		wins=$((${wins}+1))
	else
		echo -e "${RED}Wish a good luck next time${ENDCOLOR}"
		losses=$((${losses}+1))
	fi
	echo "Your rating (wins:losses):  ${wins}:${losses}"
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

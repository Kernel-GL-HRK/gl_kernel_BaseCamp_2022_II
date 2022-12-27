#!/usr/bin/bash
while true; do
	read -p "Wanna play a game?:  " choice
	if [[ ${choice} = 'y' || ${choice} = 'Y' ]]; then
		echo "+"
	elif [[ ${choice} = 'n' || ${choice} = 'N' ]]; then
		exit 0;
	elif [[ ${choice} =~ ^[0-9]+$ ]]; then
		while [[ ${choice} -gt 0 ]]; do
			echo ${choice}
			choice=$((choice-1))
		done
	fi
done

#!/bin/bash

game_path="../task1-simple-program"
game_name="guess"


if [ ! -f "$game_path/$game_name" ]; then
	echo "Can't find game executable!"
	exit 1
elif [ ! -x "$game_path/$game_name" ]; then
	echo "Game file in not executable!"
	exit 2
fi

purple='\033[1;35m'
green='\033[1;32m'
yellow='\033[1;33m'
cyan='\033[0;36m'
no_color='\033[0m'

total_tries=0
win_tries=0


run_game() {
	$game_path/$game_name
	
	if [ $? -eq "0" ]; then
		echo -e "${green}Good job!${no_color}"
		((total_tries++))
		((win_tries++))
	elif [ $? -eq "1" ]; then
		echo -e "${yellow}Good luck next time!${no_color}"
		((total_tries++))
	else
		echo "Something goes wrong..."
		exit 10;
	fi
}

echo -e "${purple}Guess the number!${no_color}"
echo

run_game

while true; do

	echo
	echo -e "${cyan}Wanna another try?${no_color}"
	read -p "Yes, No or Number of tries without this prompt (Y/n/0..99): " ans
	
	case $ans in
	[0-9] | [0-9][0-9])
		for n in $(seq $ans); do run_game; done
		;;
	[Nn])
		break
		;;
	*)
		run_game
		;;
	esac

done

echo
echo "Lucky tries: $win_tries"
echo "Total tries: $total_tries"
echo "Bye!"

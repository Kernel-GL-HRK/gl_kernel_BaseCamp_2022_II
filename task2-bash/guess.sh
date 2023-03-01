#!/bin/bash

game_path="../task1-simple-program"
game_name="guess"

# Error codes
E_CANT_FIND_EXECUTABLE=1
E_NOT_AN_EXECUTABLE=2
E_UNRECOGNIZED=10

# Error messages
e_msg["$E_CANT_FIND_EXECUTABLE"]="Cannot find game executable!"
e_msg["$E_NOT_AN_EXECUTABLE"]="Game file is not executable!" 
e_msg["$E_UNRECOGNIZED"]="Something went wrong..."

# Exit function
e_exit() {
	echo ${e_msg[$1]}
	exit $1
}

# Init checks
if [ ! -f "$game_path/$game_name" ]; then
	e_exit ${E_CANT_FIND_EXECUTABLE}
elif [ ! -x "$game_path/$game_name" ]; then
	e_exit ${E_NOT_AN_EXECUTABLE}
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
		e_exit ${E_UNRECOGNIZED}
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

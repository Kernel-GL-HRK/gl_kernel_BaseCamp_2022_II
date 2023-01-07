#!/bin/bash

COUNT=0
# Reset
Color_Off='\033[0m' # Text Reset
# Regular Colors
Black='\033[0;30m'  # Black
Red='\033[0;31m'    # Red
Green='\033[0;32m'  # Green
Yellow='\033[0;33m' # Yellow
Blue='\033[0;34m'   # Blue
Purple='\033[0;35m' # Purple
Cyan='\033[0;36m'   # Cyan
White='\033[0;97m'  # White

while true; do
    ./main.out
    # Check the result and depending on it print greetings
    if [ $? -eq 0 ]; then
	echo -e "${Green}Good job!${White}"
    else 
	echo -e "${Red}Wish a good luck next time.${White}"
    fi
    
    if [ $COUNT -eq 0 ]; then
	read -p "$(echo -e $Yellow'Continue? (Y/N) or number of tries  '$White)"  reply
	# Check if reply is not a  number
	if ! [[ $reply =~ ^[0-9]+$ ]]; then
	    if [ "$reply" = "N" ] || [ "$reply" = "n" ]; then
		break
	    fi
	else
	    let COUNT=$((reply-1))
	fi
    else
	let COUNT-=1
    fi
done
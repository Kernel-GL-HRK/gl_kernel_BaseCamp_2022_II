#!/bin/bash

COUNT=0

while true; do
    ./main.out
    # Check the result and depending on it print greetings
    if [ $? -eq 0 ]; then
	echo "Good job!"
    else 
	echo "Wish a good luck next time."
    fi
    
    if [ $COUNT -eq 0 ]; then

	read -p "Continue? (Y/N) or number of tries " reply
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
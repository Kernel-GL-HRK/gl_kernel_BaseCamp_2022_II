#!/bin/sh

while true; do
    ./main.out
    # Check the result and depending on it print greetings
    if [ $? -eq 0 ]; then
	echo "Good job!"
    else 
	echo "Wish a good luck next time."
    fi
 
    read -p "Continue? (Y/N) or number of tries " reply

    if [ "$reply" = "N" ] || [ "$reply" = "n" ]; then
	break
    fi
    

done
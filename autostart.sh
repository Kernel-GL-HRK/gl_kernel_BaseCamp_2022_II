#!/bin/sh

while true; do
    ./main.out
    
    read -p "Continue? (Y/N) or number of tries " reply
    if [ "$reply" = "N" ] || [ "$reply" = "n" ]; then
	break
    fi
    

done
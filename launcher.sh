#!/bin/bash

retcode=0
wins=0
count=1

Color_Off='\033[0m' # Text Reset
Red='\033[0;31m'    # Red
Green='\033[0;32m'  # Green
Yellow='\033[0;33m' # Yellow

while [ "$count" -gt 0 ]
do
	./guesanumber
	let retcode=$?

	if [ "$retcode" -eq 0 ]; then
		let wins=$wins+1
		echo -e "${Green}Good job${Color_Off}"
	else
		echo -e "${Yellow}Wish a good luck next time${Color_Off}"
	fi

	echo "Number of success cases: ${wins}"
done

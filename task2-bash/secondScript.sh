#!/bin/bash

COUNT=0
ANSWER=
USERCOUNT=1
ISINTEGER='^[0-9]+$'

while true
do 
	read -p  "Do u want start task1(y/n/number for repeat):" ANSWER;
	
	if [ $ANSWER == "y" ]
	then
		./../task1-simple-program/build/guess_a_number;
		if [ $? -eq 0 ]
		then
			echo "Good job";
			COUNT=$((COUNT+1));
		else
			echo "Wish a good luck next time";
		fi
	elif [[ $ANSWER =~ $ISINTEGER ]]
	then
		USERCOUNT=$ANSWER;
		while [ $USERCOUNT -ge 1 ]
		do
			./../task1-simple-program/build/guess_a_number;
			
			if [ $? -eq 0 ]
			then
				echo "Good job";
				COUNT=$((COUNT+1));
			else
				echo "Wish a good luck next time";
			fi

			USERCOUNT=$(( USERCOUNT - 1));
		done
	elif  [ $ANSWER == "n" ]
	then
		break;
	fi
done

echo "In this session you have $COUNT right answer";

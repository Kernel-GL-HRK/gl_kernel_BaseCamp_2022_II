#!/bin/bash

EXEC_PATH=../task1-simple-program
EXEC_FILENAME=a.out

COLOR_OFF='\033[0m'
COLOR_RED='\033[0;31m'
COLOR_GREEN='\033[0;32m'

#Counter of number successful attempts
success=0

#User input variable
ask=""

repeats=1

help () {
	echo
	echo ") type y to run pogram once"
	echo ") type number to run program number times without a prompt"
	echo ") type n to exit"
}


if [[ ! -f "$EXEC_PATH/$EXEC_FILENAME" ]]
then
	echo "$EXEC_FILENAME doesn't exist" >&2
	exit -1
fi

if [[ ! -x "$EXEC_PATH/$EXEC_FILENAME" ]]
then
	echo "Doesn't have execution permissions" >&2
	exit -1
fi

while true
do
	help
	read ask
	case $ask in
		[yY]|[yY][eE][sS] )
			if $EXEC_PATH/$EXEC_FILENAME
			then
				let success=$success+1
				echo -e $COLOR_GREEN"Good job"$COLOR_OFF
				echo "Successful attempts: $success"
			else
				echo -e $COLOR_RED"Wish a good luck next time"$COLOR_OFF
			fi
		;;

		[nN]|[nN][oO] )
			break
		;;

		* )
			if [[ $ask =~ ^[0-9]+$ ]]
			then
				for ((repeats=$ask; repeats > 0; repeats--))
				do
					if $EXEC_PATH/$EXEC_FILENAME
					then
						let success=$success+1
						echo -e $COLOR_GREEN"Good job"$COLOR_OFF
						echo "Successful attempts: $success"
					else
						echo -e $COLOR_RED"Wish a good luck next time"$COLOR_OFF
					fi
				done
			else
				echo "Unknown command"
			fi
		;;
	esac
done

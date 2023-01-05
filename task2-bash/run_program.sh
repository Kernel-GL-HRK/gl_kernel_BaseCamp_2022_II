#!/bin/bash

COUNT=0

run_program () {
    for ((i=0; i<$1; i++))
    do
	#run program "guess a number" from task1 in infinity loop
	../task1-simple-program/guess_a_number
	RESULT=$?

	#Also after each success script says "Good job", otherwise "Wish a good luck next time". 
	if [[ $RESULT -eq 0 ]]; then
	    let COUNT=$COUNT+1
	    echo "Good job"
	else
	    echo "Wish a good luck next time"
	fi
    done
}

COUNT=0

while true
do
    #each iteration ask user to press "y" to continue, "n" to exit, 
    #or number to set number of runs of tries without asking "y or n".
    echo " "
    echo "Enter y to continue"
    echo "Enter number to set number of runs of tries"
    echo "Enter n to exit"
    read -p "Please do your choice: " val
    case $val in
	[Yy])
	    run_program 1 ;;
	[Nn])
	    break ;;
	*)
	    run_program $val ;;
    esac
done

#Script counting users success cases and printing it numbers.
echo "Your success case: $COUNT"
echo " "

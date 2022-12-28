#!/bin/bash

# build program from task1
cmake -S ../task1
cmake --build .

# variable to read user input (y/n)
USER_CHOICE=""
# counter of guesses
GUESSES_COUNTER=0
QUESTION="Enter y to play again, n to exit or number to guess a number"

echo "Start the game..."
echo $QUESTION
while [ "$USER_CHOICE" != "n" ]
do
    ./GuessANumber
    if [ $? -eq 0 ]
    then
       echo "Good job"
       let "GUESSES_COUNTER++"
    else
       echo "Wish a good luck next time"
    fi

    echo "Right guesses: $GUESSES_COUNTER"
    echo $QUESTION

    USER_CHOICE=""
    while [[ "$USER_CHOICE" != "n" ]] && [[ "$USER_CHOICE" != "y" ]]
    do
       read USER_CHOICE
    done
done
#    case $USER_CHOICE in
#    yes
#read USER_CHOICE



#!/bin/bash

guess_game(){
USER=""
while [[ ! $USER =~ ^[0-9] ]]; do
    read -p "Guess a number [0-9] : " -n 1 USER
    echo
done

PC=$(($RANDOM % 10))
echo "User choice -" $USER
echo "  PC choice -" $PC

if [[ "$USER" == "$PC" ]];
then
    echo "You win"
    return 0
fi
echo "You loose"
return 1
}

echo "Loop Guess Game"

USER_INPUT=""
NUM_GAMES=""

while true; do

read -p "Are you want play game [y/n/Num games] : " USER_INPUT
echo "Your input is - << $USER_INPUT >>"

if ! [[ $USER_INPUT =~ ^[0-9]+$ ]]; 
then 
   case $USER_INPUT in
    [nN] )
    NUM_GAMES=0
    break;
    ;;
    [yY] )
    echo "Lets try to play game"
    NUM_GAMES=1
    ;;
    *)
    echo "Wrong input, try again"
    ;;
esac

else
   echo "You want play $USER_INPUT games?";
   NUM_GAMES=$USER_INPUT
fi

for ((gm =1 ;gm <=$NUM_GAMES;gm++));
do
echo "Game $gm of $NUM_GAMES start"
guess_game
if [[ $? == "0" ]];
then
echo "Good job"
else
echo "Wish a good luck next time"
fi

done

done

echo "Bye-bye"

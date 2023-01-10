#!/bin/bash

GAME="../debug/main"
SCORE=0


game(){
    $GAME $1
    if [ $? -eq 0 ]
    then
    echo -e "\033[0;32m Good job \033[0m"
    SCORE=$((SCORE+1))
    else
    echo -e "\033[0;33m Wish a good luck next time \033[0m"
    fi
}

while true; do

echo "Enter Y to start the game"
echo "Enter N to see the score and quit"
echo "Enter 0-9 to continue the game with the chosen number"

read -p "Enter your choice: " reply

case $reply in
  Y|YES|y|yes|Yes) game;;
  N|NO|n|no|No)  echo "Youre score: " $SCORE
         exit 0 ;;
  [0-9]) game $reply;;
  *) echo "Invalid choice!"; exit 1 ;;
esac

done
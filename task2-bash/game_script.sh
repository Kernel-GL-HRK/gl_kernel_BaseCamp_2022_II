#!/bin/bash
game="../task1-simple-program/game.out"

game_func()
{
	$game
	if [[ $? -eq 0 ]]; then
		echo "Good job"
	else
		echo "Wish a good luck next time"
	fi
}

choise='y'
while [[ $choise != 'n' ]]; do
	read -p "Wanna play a game?:  " choise
	if [[ $choise = 'y' || $choise = 'Y' ]]; then
		game_func
	elif [[ $choise = 'n' || $choise = 'N' ]]; then
		echo "Bye bye"
#check if choise is a number
	elif [[ "$choise" =~ ^[0-9]+$ ]]; then
#if yes then run the cycle
			while [[ $choise -gt 0 ]]; do
				game_func
				choise=$(($choise-1))
			done

	fi
done

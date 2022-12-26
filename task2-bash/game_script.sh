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
	fi
done

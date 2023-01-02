#!/bin/bash

game_path="../task1-simple-program"
game_name="guess"


if [ ! -f "$game_path/$game_name" ]; then
	echo "Can't find game executable!"
	exit 1
fi

$game_path/$game_name

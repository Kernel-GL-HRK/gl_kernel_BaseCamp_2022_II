#!/bin/bash

echo "Loading game..."
tar xf ./release/guessanumber.tar.gz  -C ./release/
cd ./release/tmp/guessanumber/
gcc guess.c -o game



guest="y"

while [ $guest != "n" ]
	do
		./game
		echo "Continue? y/n"
		read guest
	done
echo "Bye!"

rm game
rm guess.c
cd ..
rmdir guessanumber
cd ..
rmdir tmp
cd ..


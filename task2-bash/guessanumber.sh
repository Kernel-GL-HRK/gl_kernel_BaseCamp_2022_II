#!/bin/bash

echo "Loading game..."
tar xf ./release/guessanumber.tar.gz  -C ./release/
cd ./release/tmp/guessanumber/
gcc guess.c -o game

until [ "$guest" = "n" ]; do

echo "	y) play"
echo "	n) exit"
echo " number) number of rounds"

read guest

case $guest in
	y)
		./game
		;;
	n)
		echo "Bye!"
		exit 1
		;;
	[0-9]|[0-9][0-9])
		wincounter=0
		while [ "$guest" != 0 ] 
		do
			./game 
			if [ $? == 0 ]
			then
				(( wincounter++ ))
				echo "Good job!"
			else
				echo "Wish a good luck next time"
			fi
			(( guest-- ))
		done 
		echo You win $wincounter times
		;;
	*) 
		echo "Invalid input" 
		;;
esac
done

rm game
rm guess.c
cd ..
rmdir guessanumber
cd ..
rmdir tmp
cd ..


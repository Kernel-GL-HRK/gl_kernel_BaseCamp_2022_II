#!/bin/bash
if [ -d /tmp/guesanumber ]
then

	cp -r ../task1-simple-program/* /tmp/guesanumber;

else

	mkdir /tmp/guesanumber
	cp -r ../task1-simple-program/* /tmp/guesanumber

fi

tar -cvzf guesanumber.tar /tmp/guesanumber 
if [ -d release ]
then

	cp guesanumber.tar ./release

else

	mkdir release;
	cp guesanumber.tar ./release

fi

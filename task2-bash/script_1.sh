#! /bin/bash

SOURCE="../task1-simple-program"
TARGET="/tmp/guessanumber"

echo "Creating folder $TARGET . . ."
mkdir $TARGET

if [ $? -eq 0 ]
then
	echo "Folder created SUCCESSFULLY."
else
	echo "Creating folder is FAILED!"
fi

echo "Copying folder to /tmp . . ."
cp -r $SOURCE/* $TARGET

if [ $? -eq 0 ]
then
	echo "Copying is SUCCESSED."
else
	echo "Copying is FAILED!"
fi

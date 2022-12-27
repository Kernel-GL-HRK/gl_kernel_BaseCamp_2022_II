#! /bin/bash

SOURCE="../task1-simple-program"
FOLDER="guessanumber"
TARGET="/tmp/"$FOLDER


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

RELEASE=$(pwd)/release
ARCHIVE=$TARGET.tar.gz

tar czvf $ARCHIVE $TARGET

mkdir release

if [ $? -eq 0 ]
then
	echo "Folder created SUCCESSFULLY."
else
	echo "Creating folder is FAILED!"
fi

cp $ARCHIVE $RELEASE/

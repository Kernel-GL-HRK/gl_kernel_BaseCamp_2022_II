#!/bin/bash

echo Backup source code

BACKUPDIR="/tmp/guesanumber"
SOURCEDIR="../task1-simple-program"
RELEASEDIR=$PWD/release

echo Backup on $BACKUPDIR

echo "=========="

if [[ -d "$BACKUPDIR" ]]; then
	echo "Directory $BACKUPDIR exists."
	echo "Clean $BACKUPDIR"
	rm -rf $BACKUPDIR/*
else
	echo "Directory $BACKUPDIR does not exists."
	echo "Create $BACKUPDIR"
	mkdir $BACKUPDIR
fi

echo "=========="

echo "Copy project files to $BACKUPDIR"
cp $SOURCEDIR/* $BACKUPDIR/
echo "List of files:"
ls -ld $BACKUPDIR/*

echo "=========="

if [[ -d "$RELEASEDIR" ]]; then
	echo "Directory $RELEASEDIR exists."
	echo "Clean $RELEASEDIR"
	rm -rf $RELEASEDIR/*
else
	echo "Directory $RELEASEDIR does not exists."
	echo "Create $RELEASEDIR"
	mkdir $RELEASEDIR
fi

echo "=========="

echo "GZIP files"
cd $BACKUPDIR
tar -czvf $RELEASEDIR/guess_game.tar.gz *
cd $OLDPWD

echo "=========="

echo "DONE"

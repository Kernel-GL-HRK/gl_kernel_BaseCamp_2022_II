#! /bin/bash

if [[ ! -d /tmp/guessanumber ]]
then
        mkdir /tmp/guessanumber
fi

cp ../task1-simple-program/*.c /tmp/guessanumber

tar -zcvf guessanumber.tar.gz /tmp/guessanumber

if [[ ! -d ./release ]]
then
        mkdir release
fi

mv guessanumber.tar.gz release/


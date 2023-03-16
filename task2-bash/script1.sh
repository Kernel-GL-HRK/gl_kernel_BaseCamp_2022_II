#! /bin/bash

mkdir /tmp/guessanumber

cp ../task1-simple-program/*.{c, h} /tmp/guessanumber

tar -zcvf guessanumber.tar.gz /tmp/guessanumber

mkdir release

mv guessanumber.tar.gz release/


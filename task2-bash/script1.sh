#! /bin/bash

cp ../task1-simple-program/*.c /tmp/guessanumber

tar -zcvf guessanumber.tar.gz /tmp/guessanumber

mv guessanumber.tar.gz release/

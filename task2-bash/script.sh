#!/bin/bash

mkdir -p tmp/guessanumber

cp ../task1-simple-program/*.c ./tmp/guessanumber/ 

tar czf guessanumber.tar.gz ./tmp/guessanumber

mkdir release

cp guessanumber.tar.gz ./release/

rm guessanumber.tar.gz

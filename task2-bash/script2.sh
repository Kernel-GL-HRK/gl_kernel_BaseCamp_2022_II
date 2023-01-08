#!/bin/bash

GAME=../a.out

while true; do
    ./$GAME
    if [ $? -eq 0 ]; then
        echo "Good job"
    else
        echo "Wish a good luck next time"
    fi
done
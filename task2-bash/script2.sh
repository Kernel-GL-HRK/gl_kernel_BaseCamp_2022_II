#!/bin/bash

GAME=../a.out
loop_count=1

while true; do
    ./$GAME
    if [ $? -eq 0 ]; then
        echo "Good job"$'\n'
    else
        echo "Wish a good luck next time"$'\n'
    fi
    ((loop_count--))
    if [ $loop_count -lt 1 ]; then
        read -p "Continue y/n. Or number of loops: " answ
        case $answ in
            [Yy])
                continue
                ;;
            [Nn])
                break
                ;;
            *)
                ((loop_count=answ))
                ;;
        esac
    fi
done
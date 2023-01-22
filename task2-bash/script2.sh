#!/bin/bash

GAME=../a.out
loop_count=1
sucess_count=0

while true; do
    ./$GAME
    if [ $? -eq 0 ]; then
        echo "Good job"$'\n'
        ((sucess_count++))
    else
        echo "Wish a good luck next time"$'\n'
    fi

    echo "Correctly guessed $sucess_count times"$'\n'
    
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
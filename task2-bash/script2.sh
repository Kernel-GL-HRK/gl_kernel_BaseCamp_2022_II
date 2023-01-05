#!/bin/bash

GAME=../task1-simple-program/guess_a_number

win_count=0

game() {
        $GAME
        case $? in

                0)
                        win_count=$(($win_count+1))
                        echo "Good job"
                        ;;

                1)
                        echo "Wish a good luck next time"
                        ;;

                2)
                        echo "You lost your attempt. Next time be careful"
                        ;;

        esac
}     

game

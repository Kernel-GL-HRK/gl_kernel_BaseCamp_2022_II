#!/bin/bash

GAME=../task1-simple-program/guess_a_number

win_count=0
restart_count=1

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

while :                                                                         
do                                                                              
                                                                                
echo "WIN COUNT      = $win_count"                                              
echo "RESTART COUNT  = $restart_count"                                          
                                                                                
if [ $restart_count -gt 0 ]                                                     
then                                                                            
        start_game                                                              
        restart_count=$(($restart_count-1))                                     
        continue                                                                
fi                                                                              
                                                                                
done          

#!/bin/bash
# guess a number

loop=1
game=~/gl_base_camp/gl_kernel_BaseCamp_2022_II/task1-simple-program/main
success_count=0

RED='\033[1;31m'
BGREEN='\x1b[1;32m'
BRED='\033[1;31m'
NOC='\033[0m'

while [ 1 ]
do
        for (( i = 0; i < loop; i++ ))
        do
                $game
                if [ $? -eq 0 ]
                then
                        echo "Good job"
                        ((success_count++))
                else
                        echo "Wish a good luck next time"
                fi

                echo -e "${BGREEN}Success cases $success_count ${NOC}"
        done

        echo "To continue press \"y\", \"n\" or number of tries"
        read var

        if [[ $var = "y" ]]
        then
                loop=1
        elif [[ $var = "n" ]]
        then
                break
        elif [[ "$var" =~ ^[0-9]+$ ]]
        then
                loop=$var
        else
                loop=0
                echo -e "${BRED}Error input!${NOC}"
        fi
done

#!/bin/bash
# guess a number

loop=1
game=~/gl_base_camp/gl_kernel_BaseCamp_2022_II/task1-simple-program/main

while [ 1 ]
do
        for (( i = 0; i < loop; i++ ))
        do
                $game
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
                echo "Error input!"
        fi
done

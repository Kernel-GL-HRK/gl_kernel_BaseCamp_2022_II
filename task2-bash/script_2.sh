#!/bin/bash
# script_2.sh
# guess a number

loop=1

while [ 1 ]
do
        echo "To continue press \"y\", \"n\" or number of tries"
        read var
        if [[ $var = "y" ]]
        then
                echo "Infinity loop test"
        elif [[ $var = "n" ]]
        then
                break
        elif [[ "$var" =~ ^[0-9]+$ ]]
        then
                loop=$var
                echo "number = $loop"
        else
                echo "Error input!"
        fi
done

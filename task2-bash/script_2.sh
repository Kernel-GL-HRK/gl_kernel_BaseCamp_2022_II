#!/bin/bash
# script_2.sh
# guess a number

while [ 1 ]
do
        read var
        if [[ $var = "y" ]]
        then
                echo "Infinity loop test"
        elif [[ $var = "n" ]]
        then
                break
        fi
done

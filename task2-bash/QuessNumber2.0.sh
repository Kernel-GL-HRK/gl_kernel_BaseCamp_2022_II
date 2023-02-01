#!/bin/bash

SOURCE_FILE="main.c"
SOURCE_FOLDER="./"
OUTPUT_FILE=${SOURCE_FILE%??}.out

if [ -d $SOURCE_FOLDER ]; then
    cd $SOURCE_FOLDER
else
    echo "Directory with program was't found"
    exit
fi
if [ -f $SOURCE_FILE ]; then
    gcc $SOURCE_FILE -o $OUTPUT_FILE
else
    echo "Program was't found"
    exit
fi

while true; do
    ./$OUTPUT_FILE
    echo
    read -p "Do you want continue?(y|n):" YesNo
    case $YesNo in
        Y|y)
            continue
        ;;
        N|n)
            break
        ;;
        [0-9])
            for (( i=1; i < $YesNo; i++ )); do
                ./$OUTPUT_FILE
                echo
            done
        ;;
        *)
        echo "!!!Wrong answer!!!"
        exit
        ;;
    esac
done
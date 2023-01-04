#! bin/bash

BLUE='\033[0;34m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m'

PATH=../task1-simple-program

if [[ ! -f ${PATH}/main.out ]]; then
        gcc ${PATH}/main.c -o ${PATH}/main.out
fi

counter=1
wins=0
looses=0

until [[ $counter -eq 0 ]]; do
        echo -e ${BLUE}"Tries: ${counter}\t Wins: ${wins}\t Looses: ${looses}"${NC}

        ((--counter))

        ${PATH}/main.out
        ret=$?

        if [ $ret -eq 0 ]; then
                echo -e ${GREEN}"Good job"${NC}
                ((++wins))
        elif [ $ret -eq 1 ]; then
                echo -e ${RED}"Wish a goog luck next time"${NC}
                ((++looses))
        fi

        if [ $counter -eq 0 ]; then
        		echo -n -e ${YELLOW}"Want to continue (Y/N) | Number of tries ->"${NC}
                read reply

                case $reply in

                No|NO|N|n) ;;

                ("" | *[!0-9]*) ((++counter)) ;;

                (*) counter=$reply ;;

                esac
        fi
done

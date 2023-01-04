#! bin/bash

counter=1
wins=0
looses=0

until [[ $counter -eq 0 ]]; do
	echo "Tries: ${counter}\t Wins: ${wins}\t Looses: ${looses}"
        ((--counter))

        ../task1-simple-program/main.out
        ret=$?

        if [ $ret -eq 0 ]; then
                echo -e "Good job"
                ((++wins))
        elif [ $ret -eq 1 ]; then
                echo -e "Wish a goog luck next time"
                ((++looses))
        fi

        if [ $counter -eq 0 ]; then
        		echo -n -e "Want to continue (Y/N) | Number of tries ->"
                read reply

                case $reply in

                No|NO|N|n) ;;

                ("" | *[!0-9]*) ((++counter)) ;;

                (*) counter=$reply ;;

                esac
        fi
done

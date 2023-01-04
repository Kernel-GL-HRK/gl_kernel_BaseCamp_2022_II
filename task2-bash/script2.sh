#! bin/bash

counter=1

until [[ $counter -eq 0 ]]; do

        ((--counter))

        ../task1-simple-program/main.out
        ret=$?

        if [ $ret -eq 0 ]; then
                echo "Good job"
        elif [ $ret -eq 1 ]; then
                echo "Wish a goog luck next time"
        fi

        if [ $counter -eq 0 ]; then
        	echo -n "Want to continue (Y/N) | Number of tries ->"
                read reply

                case $reply in

                No|NO|N|n) ;;

                ("" | *[!0-9]*) ((++counter)) ;;

                (*) counter=$reply ;;

                esac
        fi
done

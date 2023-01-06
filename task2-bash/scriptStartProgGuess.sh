#! /bin/bash

ON_IRED='\033[0;101m'
ON_IBLUE='\033[0;104m'
ON_IGREEN='\033[0;102m'
ON_IBLACK='\033[0;100m'
IGREEN='\033[0;92m'     
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
GREEN='\033[0;32m'
DARKGRAY='\033[1;30m'
BWHITE='\033[1;37m'
BBLACK='\033[1;30m' 
UWHITE='\033[4;37m'      
RED='\033[0;31m'  
NC='\033[0m' # No Color

source=../task1-simple-program

PROGRAM=mains # Name
let MAXNUM=30
let result_game=0 # game result
let Try=0
let Lose=0
let Win=0
let counter=0
let try=0
Stop="Y"

program_guess () {
  $source/$PROGRAM -t
  let result_game=$?
  if [ "$result_game" -eq "0" ]; then
    let Win++
    let Try++
    echo -e "   ${ON_IGREEN}            Good job!               ${NC}  "
  else
    let Lose++
    let Try++
    echo -e "   ${ON_IRED}    Wish a good luck next time.     ${NC}"
  fi
}

total () {
  clear
  echo -e "\n${ON_IBLACK} * ${NC}${ON_IBLUE}        GAME \"Guess number!\"        ${NC}"
  echo -e "${ON_IBLACK} * ${NC}${ON_IBLUE}     Total      WIN       LOSE      ${NC}"
  echo -e "   ${BWHITE}       $Try         $Win         $Lose    ${NC}"
  for (( i=1; i<=Try; i++ ))
  do
    echo -en "${ON_IBLACK} * ${NC}     ${ON_IBLUE}      ${NC}  "
    if [ "$Win" -gt 0 ]; then
      echo -en "  ${ON_IGREEN}      ${NC}  "
      let Win--
    else
        echo -en "          "
    fi
    if [ "$Lose" -gt 0 ]; then
      echo -e "  ${ON_IRED}      ${NC}"
      let Lose--
    else
      echo -e " "  
    fi
  done
  echo
}
clear
echo ""
echo "Game \"Guess a number\""
echo "Startup type:"
PS3='Choose an option: '
select start in "loop" "run N" "quit" 
do
  case $REPLY in
    1)
        until [ $Stop = "N" ]; do
        clear
        program_guess
        let try++
        echo -e "   ${ON_IBLUE} *${try} ${NC}  ${ON_IGREEN} +${Win} ${NC}  ${ON_IRED} -${Lose} ${NC}"
        read -p "exit n | N or Press [Enter] key / (10 seconds) to continue ..." -t 10 reply
        Stop=`echo $reply | tr [:lower:] [:upper:]`
        done
        total
        exit 0;;
    2)
        while [ True ]
        do 
          read -p "how many times to run the game from 1 to ${MAXNUM} : " counter;
          if [[ $counter =~ ^[0-9]+$ && $counter -le ${MAXNUM} ]]; then
            break 
          fi
        done
        let try=counter
        while [ $counter -gt 0 ] 
        do 
          clear
          program_guess
          let counter=$counter-1 
          echo -e "   ${ON_IBLUE} Total:${try} *${counter} ${NC}  ${ON_IGREEN} +${Win} ${NC}  ${ON_IRED} -${Lose} ${NC}" 
          read -s -p "Press any key to continue or wait 3 seconds..." -t 3
        done
        total
        exit 0;;
    3)
      echo "Tschüss bis später."
      exit 0;;
    *) 
      echo "Invalid entry $REPLY"
      exit 0;;
  esac
done

echo "Ende!"
exit 0
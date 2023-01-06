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
  else
    let Lose++
    let Try++
  fi
}


#!/bin/bash

if [[ -z "$1" ]] || [[ -z "$2" ]]
  then
	
	echo "************************No arguments********************"
	echo "Please provide led letter \"r/g/b\" as first argument" 
	echo "and blink delay in (ms) as second"
	echo "set delay to 0 to disable the led"
	echo "*******************************************************"
	exit 1
  fi


set -v

echo $1 $2 > /dev/leds_rgb
echo $3 $4 > /dev/leds_rgb
echo $5 $6 > /dev/leds_rgb


cat /proc/project1/leds_rgb
 

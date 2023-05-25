#!/usr/bin/bash
sudo bash -c "cat main.c > /dev/mymod"
sudo bash -c "cat mymod.c > /dev/mymod"
sudo bash -c "cat main.c > /dev/mymod"

sudo bash -c "cat /dev/mymod"

cat /sys/kernel/mymod_cdev/mymod
cat /sys/kernel/mymod_cdev/mymod

sudo ./main.out &
sudo bash -c "cat /dev/mymod" 
sudo bash -c "cat /dev/mymod"



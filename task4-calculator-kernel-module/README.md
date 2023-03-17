# calculator_module
Simple example of Linux Kernel Module demonstrating passing parameters to kernel module

 - [Building the Module](#building-the-module) 
 - [Installing/Uninstalling ](#install-uninstall) 

## Building the Module
```sh
make [KDIR=[Path to your linux kernel]]
```
The module is compiled to `calculator_module.ko`.


## Installing/Uninstalling
```sh
# Install with parameters:
sudo insmod calculator_module.ko parm1=[First int] parm2=[Second int]
```
```sh
# Uninstall:
sudo rmmod calculator_module.ko
```
Note: if `parm1` or `parm2` are equal 0 or not provided, you get the following error:
```sh
insmod: ERROR: could not insert module calculator_module.ko: Invalid parameters
```
## View the results/ running calculator
```sh
sudo dmesg | tail
```

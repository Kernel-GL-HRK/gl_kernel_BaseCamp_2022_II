
##### Simple "LED Blinker" module with procfs

This module can switch the state of chosen gpio pin with 
specified intervals. Which can be used for blinking a LED.

Module has parameters described below:
**gpio** - GPIO pin (If not set default value is 26)
**on** - Interval for HIGH pin state in millisecs (If not set default is 500)
**off** - Interval for LOW pin state in millisecs (If not set default is 500)

Module creates informational (ro) file in procfs in its own directory.
For example it can be **/proc/blinker/gpio26** where **26** is GPIO pin.

This file displays the following information:
- Active GPIO pin
- HIGH pin state duration in millisecs
- LOW pin state duration in millisecs
- Current state
- Current timeout
- Total number of state switches

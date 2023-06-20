<h1 style="text-align: center;">SMART-CLOCK</h1>

<a href="url"><img src="https://github.com/Eddie07/smart-clock/blob/main/blob/media/device.jpg?raw=true" align="center" height="600" width="600" ></a>	

# Kernel device manual

**Contents:**

[**1. Hardware connection**](#1-hardware-connection)

[**1.1 Hardware connection to Raspberry 40 pin extension.**](#hardware-connection-to-raspberry-40-pin-extension)

[**1.2. Enable SPI, I2C and DTBO in Raspberry config.**](#12-enable-spi-i2c-and-dtbo-in-raspberry-config)

[**1.3. Disable time synchronization on Raspberry.**](#13-disable-time-synchronization-on-raspberry)

[**1.4. Module installation.**](#14-module-installation)

[**1.5. Removing the module from memory**](#15-remove-the-module-from-memory)

[**2. User manual**](#2-user-manual)

[**2.1 Main functions**](#21-main-functions)

[**2.1.1. Time**](#211-time)

[**2.1.2. Timer**](#212-timer)

[**2.1.3. Alarm**](#213-alarm)

[**2.1.4. Temperature and Pressure**](#214-temperature-and-air-pressure)

[**2.1.5. Pedometer**](#215-pedometer)

[**2.1.6. Game**](#216-game)

[**2.1.7. Options**](#217-options)

[**2.1.8. Making of screenshots**](#218-making-of-screenshot-bmp-format)

[**2.1.9. Display BMP files**](#219-display-bmp-files)

[**2.1.10. Notifications bar**](#2110-notifications-bar)

---

## 1. Hardware connection


### Hardware connection to Raspberry 40 pin extension:

<a href="url"><img src="https://github.com/Eddie07/smart-clock/blob/main/blob/media/display_connection.png?raw=true" align="right" height="250" width="350" ></a>

#### __Connect TFT Module (SPI):__

VDD3.3-\> 3,3v (pin 1)

CS -\> SPI0_CE0 (pin 24)

SCL -\> SPI0_SCLK (pin 23)

SDA -\> SPI0_MOSI (pin 19)

RS -\> GPIO_23 (pin 16)

RST -\> GPIO_24 (pin 18)

GND -\> GROUND (pin 25)

<a href="url"><img src="https://github.com/Eddie07/smart-clock/blob/main/blob/media/bmp280_connection.png?raw=true" align="right" height="240" width="350" ></a>

#### __Connect bmp280 sensor (I2C):__

VIN -\> 3,3v (pin 1)

GND -\> GROUND (pin 25)

SCL -\> SCL1 i2c (pin 5)

SDA -\> SDA1 i2c (pin 3)

&nbsp;

&nbsp;

#### __Connect mpu6050 sensor (I2C):__
<a href="url"><img src="https://github.com/Eddie07/smart-clock/blob/main/blob/media/mpu6050_connection.png?raw=true" align="right" height="250" width="350" ></a>

VCC-\> 3,3v (pin 1)

GND -\> GROUND (pin 25)

SCL -\> SCL1 i2c (pin 5)

SDA -\> SDA1 i2c (pin 3)

AD0 -\> 3,3v (pin 1)

&nbsp;

#### __Connect rtc3231 (w/o eeprom) (I2C):__
<a href="url"><img src="https://github.com/Eddie07/smart-clock/blob/main/blob/media/rtc3231_connection.png?raw=true" align="right" height="250" width="350" ></a>

\+   -\> 3,3v (pin 1)

\-  -\> GROUND (pin 25)

C -\> SCL1 i2c (pin 5)

D -\> SDA1 i2c (pin 3)

&nbsp;

&nbsp;

#### __Connect hardware button (2 pin):__
<a href="url"><img src="https://github.com/Eddie07/smart-clock/blob/main/blob/media/button_connection.png?raw=true" align="right" height="250" width="350" ></a>

Pin1 -\> 3,3v (pin 1)

Pin2 -\> GPIO_17 (pin 11)

&nbsp;

&nbsp;

&nbsp;

### 1.2. Enable SPI, I2C and DTBO in Raspberry config:

you can enable SPI, I2C from the **/boot/config.txt** file. Open a
terminal window and enter:

`sudo nano /boot/config.txt`

Look for a line that reads **#dtparam=spi=on** and remove
the **\#** symbol. Do the same for **#dtparam=i2c=on** and

**dtparam=i2c_vc=on**. Now add a line with
**dtoverlay=smart_clock.dtbo** so dtbo overlay will be connected at boot
time

Reboot your Pi.

### 1.3. Disable time synchronization on Raspberry.

By default, Raspberry is using time-synchronization service while
connected to network.

We need to disable it, because time we use from rtc clock.

`sudo systemctl stop systemd-timesyncd`

`sudo systemctl disable systemd-timesyncd`

### 1.4. Module installation

Building the Module and device-tree overlay:

1)  make \[KDIR=\[Path to your linux kernel\]\]

2)  The module will be compiled to **smart_clock.ko**

3)  The device tree overlay will be compiled to **smart_clock.dtbo**

4)  Copy **smart_clock.dtbo** to boot/overlays directory to the device

5)  Inserting into your kernel:

Type command in terminal:

`sudo insmod smart_clock.ko`

After check kernel log if device is properly booted:

>smart_clock: smart_clock: Major = 238 Minor = 0

>smart_clock: gpio_button: found gpio #17 value in DT

>smart_clock: st7735fb: init

>smart_clock: st7735fb: probe started

>st7735fb spi0.0: SPI ok

>smart_clock: st7735fb: Total vm initialized 40960

>smart_clock: bmp280: probing

>smart_clock: bmp280: probed

>smart_clock: ds3231: probing

>smart_clock: ds3231: probed

>smart_clock: ds3231: set time of the day from RTC result 0

>smart_clock: mpu6050: probing

>smart_clock: mpu6050: probed

*if some of devices will fail to start, you will get the message with
hardware name: probe failed. Check hardware connection or config.txt*

*If "set time of the day from RTC" result is other than 0 than your rtc
is not activated or battery is too low, once you setup clock or alarm
this will be fixed automatically.*

### 1.5 Remove the module from memory

Command:

`sudo rmmod smart_clock.ko`

module will be removed when the last timer is stopped. Check kernel log
if device is properly removed:

>smart_clock: controls unloaded

>smart_clock: sensors unloaded

>smart_clock: gpio_button: device exit

>smart_clock: st7735fb: exiting

>smart_clock: st7735fb: unloaded

>smart_clock: module unloaded

---

## 2. User manual

### 2.1 Main functions:

#### Smart clock is designed to work as simple smart-watch with
entertainment part. Functions:

-Time

-Alarm

-Timer

-Temperature and Pressure

-Pedometer (Steps meter)

-Game

-Making of screenshot (BMP format) (2.1.7)

-Display BMP files (2.1.7)

#### Customization is also possible through options:

-24h or AM/PM

-Temperature view in C/F

-Enable or disable the Alarm

#### Functions are controlled by one button:

-click (Short press)

-middle-long (Press and hold about 1 sec)

-long click (Press and hold longer than 1 sec)

#### 2.1.1. Time

![](https://github.com/Eddie07/smart-clock/blob/main/blob/media/clock_24.png?raw=true)    ![](https://github.com/Eddie07/smart-clock/blob/main/blob/media/clock_am.png?raw=true)

Hold the button longer than 1 sec to enter first view mode.

Current time will be loaded from "rtc"

Clock view can be customized by showing time in format 24h or AM/PM

In **Options** view.

*Adjusting the time:*

Use middle-long button action to enter adjustments mode.

Adjustment's part will be blinking. Press short the button to adjust.

To switch the adjustments to the next part hold button long click

1st adjustments will be minutes, 2nd - hours, 3rd day of the
month, 4th the month, 5th year (year is capped to 2050)

*Once you finish the adjustments - clock value will be automatically
stored in rtc.*

Use long click to switch mode to next mode: **Timer**

#### 2.1.2. Timer

![](https://github.com/Eddie07/smart-clock/blob/main/blob/media/timer.png?raw=true) 

Timer can be **started and stopped** by short pressing of the button.

**To reset** the timer -stop timer and use middle-long button action.

Timer can be **run in background** while view is switched. Use
long-press button action while timer is running to switch the view while timer is
running in background. The notification of running timer will be shown
in notification bar.

Switch to the next view **Alarm** press long press button.

#### 2.1.3. Alarm

![](https://github.com/Eddie07/smart-clock/blob/main/blob/media/alarm.png?raw=true)

The alarm can be turned on and off in **Options** view.

When Alarm is triggered the message "ALARM !!!" will appear blinking on
screen, short press the button to disable it.

*Adjusting the alarm*

Use middle-long click to enter adjustments mode.

Adjustment's part will be blinking. Press short the button to adjust.

To switch the adjustments to the next part hold button long action

1st adjustments will be minutes, 2nd -- hours

*Once you finish the adjustments -- alarm values will be automatically
stored in rtc.*

#### 2.1.4. Temperature and air pressure.

![](https://github.com/Eddie07/smart-clock/blob/main/blob/media/temp_press.png?raw=true) ![](https://github.com/Eddie07/smart-clock/blob/main/blob/media/temp_f.png?raw=true)

Temperature can be shown in C or F. Check **Options** for the option.

Switch the next view **Pedometer** press long-press button.

<a name="clock_pedom">
</a>

#### 2.1.5. Pedometer.

![](https://github.com/Eddie07/smart-clock/blob/main/blob/media/pedometer.png?raw=true)

Pedometer or Step meter shows steps (or vertical shakes) from the device
start.

To reset the pedometer short press the button.

Switch the next view **Game** press long-press button.

#### 2.1.6. Game

![](https://github.com/Eddie07/smart-clock/blob/main/blob/media/game.png?raw=true)

Besides of pedometer the mpu6050 is also used for the control of the
game "SNAKE".

Tilt the device left, right, up, down to control the snake and eat the
fruit.

Once snake eats herself or touches the white merges -- the game will be
over.

To Restart game short press the button.

Switch the next view **Options** press long-press button.

#### 2.1.7. Options

![](https://github.com/Eddie07/smart-clock/blob/main/blob/media/options.png?raw=true)

*Adjusting the options:*

Use middle-long click to enter adjustments mode.

Adjustment's part will be blinking. Press short the button to adjust.

To switch the adjustments to the next part hold button long click

*Note: Once you finish the adjustments -- alarm values will be
automatically stored in rtc.*

#### 2.1.8. Making of screenshot (BMP format)

Use terminal in your Raspberry to type the command:

`cat dev/smart_clock`

to get file stream of bmp image of your current screen.

Example:

`cat dev/smart_clock > screen_shot.bmp`

It will create new file screen_shot.bmp with the screenshot from your device.

#### 2.1.9. Display BMP files.

Use terminal in your Raspberry to print to device the command:

`cat screen.bmp > dev/smart_clock`

The file will be displayed on screen.

*Note: Only files with resolution less than 160x128 are supported.
Otherwise, you will receive the error message.*

#### 2.1.10. Notifications bar.

![](https://github.com/Eddie07/smart-clock/blob/main/blob/media/notif.png?raw=true)

When alarm is enabled in options -- the icon "Alarm" will be shown is
notification bar on screen.

When timer is running in background -- the icon "Clock" will be blinking
in notification bar on screen.

---
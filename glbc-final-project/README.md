# **GlobalLogic BaseCamp final project**  

## **1. mpu6050 gyroscope**  
### 1.1 The project uses the GY-521 gyroscope module.  
    Output AD0 - connected to a power source, in this configuration the gyroscope  
    has an i2c address of 0x69.  
    (Some GY-521 module with a RED power LED have a DEVICE ID of 0x98).  

### 1.2 File gl_mpu6050-overlay.dts - modified with address 0x69 and driver name gl_mpu6050.  
    For compile gl_mpu6050.dts to .dtbo - use make dtb.  
    For correct operation, the file gl_mpu6050.dtbo must be located in  
    the raspberry pi /boot/overlays.  
    To enable the overlay in the system, add dtoverlay=gl_mpu6050 to config.txt.  

### 1.3 Implemented sysfs, read only.  
    Raw accelerometer data is stored in the /sys/devices/mpu6050/accel/accel_x,  
    .../accel_y or .../accel_z.     
    Raw gyroscope data is stored in the /sys/devices/mpu6050/gyro/gyro_x,     
    .../gyro_y or .../gyro_z.     
    Temperature data is stored in the /sys/devices/mpu6050/temp - in degrees     
    Celsius.     

### 1.4 Add simply motion detector.     
    Movement is tracked by two coordinates (x, y).     
    We use a simple method to determine the direction, it is based on the     
    position of the accelerometer. These coordinates are in the range  
    from 0 to +/- 16340, the sensitivity threshold for an example,  
    let's choose 8000. If the sensitivity threshold is exceeded, we register  
    the direction left, right, forward, backward.  
    /sys/devices/mpu6050/direct - is used to export data to user space  
    2 - moving forward,  
    8 - back,  
    4 - left,  
    6 - right,  
    0 - no moving.  
  
## **2. 4x4 matrix keyboard**  
### 2.1 Initialization the gpio's column (output) and row (input) for keyboard.  
    A simple test was used by turning on the LED when the module was loading  
    and turning it off after exiting the module.  
### 2.2 Timer added.  
    An LED is used for testing, the ON/OFF period is 100ms.  
### 2.3 Keypad connection:      
    Keypad pinout:  1 <-> ROW3    (GPIO16) Raspberry pi.  
                    2 <-> ROW2    (GPIO25)  
                    3 <-> ROW1    (GPIO24)  
                    4 <-> ROW0    (GPIO23)  
                    5 <-> COL3    (GPIO 5)  
                    6 <-> COL2    (GPIO22)  
                    7 <-> COL1    (GPIO27)  
                    8 <-> COL0    (GPIO17)  
    The keyboard is polled every 100 ms.  
    The algorithm does not track long presses or presses of several keys  
    at the same time.  
    

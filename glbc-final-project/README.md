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


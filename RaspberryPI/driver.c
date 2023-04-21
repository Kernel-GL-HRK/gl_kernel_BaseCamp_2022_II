/***************************************************************************//**
*  driver.c
*  Simple GPIO driver explanation
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h>  //copy_to/from_user()
#include <linux/gpio.h>     //GPIO
#include <linux/err.h>

//LED is connected to this GPIO
#define GPIO_19 (19)
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev raspi_gpio_cdev;
 
static int __init raspi_gpio_driver_init(void);
static void __exit raspi_gpio_driver_exit(void);
 
 
/*************** Driver functions **********************/
static int raspi_gpio_open(struct inode *inode, struct file *file);
static int raspi_gpio_release(struct inode *inode, struct file *file);
static ssize_t raspi_gpio_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t raspi_gpio_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
/******************************************************/
 
//File operation structure 
static struct file_operations fops =
{
  .owner          = THIS_MODULE,
  .read           = raspi_gpio_read,
  .write          = raspi_gpio_write,
  .open           = raspi_gpio_open,
  .release        = raspi_gpio_release,
};

/*
** This function will be called when we open the Device file
*/ 
static int raspi_gpio_open(struct inode *inode, struct file *file)
{
  pr_info("Device File Opened...!!!\n");
  return 0;
}

/*
** This function will be called when we close the Device file
*/
static int raspi_gpio_release(struct inode *inode, struct file *file)
{
  pr_info("Device File Closed...!!!\n");
  return 0;
}

/*
** This function will be called when we read the Device file
*/ 
static ssize_t raspi_gpio_read(struct file *filp, 
                char __user *buf, size_t len, loff_t *off)
{
  uint8_t gpio_state = 0;
  
  //reading GPIO value
  gpio_state = gpio_get_value(GPIO_19);
  
  //write to user
  len = 1;
  if( copy_to_user(buf, &gpio_state, len) > 0) {
    pr_err("ERROR: Not all the bytes have been copied to user\n");
  }
  
  pr_info("Read function : GPIO_19 = %d \n", gpio_state);
  
  return 0;
}

/*
** This function will be called when we write the Device file
*/ 
static ssize_t raspi_gpio_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
  uint8_t rec_buf[10] = {0};
  
  if( copy_from_user( rec_buf, buf, len ) > 0) {
    pr_err("ERROR: Not all the bytes have been copied from user\n");
  }
  
  pr_info("Write Function : GPIO_19 Set = %c\n", rec_buf[0]);
  
  if (rec_buf[0]=='1') {
    //set the GPIO value to HIGH
    gpio_set_value(GPIO_19, 1);
  } else if (rec_buf[0]=='0') {
    //set the GPIO value to LOW
    gpio_set_value(GPIO_19, 0);
  } else {
    pr_err("Unknown command : Please provide either 1 or 0 \n");
  }
  
  return len;
}

/*
** Module Init function
*/ 
static int __init raspi_gpio_driver_init(void)
{
  /*Allocating Major number*/
  if((alloc_chrdev_region(&dev, 0, 1, "raspi_gpio_Dev")) <0){
    pr_err("Cannot allocate major number\n");
    goto r_unreg;
  }
  pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
  /*Creating cdev structure*/
  cdev_init(&raspi_gpio_cdev,&fops);
 
  /*Adding character device to the system*/
  if((cdev_add(&raspi_gpio_cdev,dev,1)) < 0){
    pr_err("Cannot add the device to the system\n");
    goto r_del;
  }
 
  /*Creating struct class*/
  if(IS_ERR(dev_class = class_create(THIS_MODULE,"raspi_gpio_class"))){
    pr_err("Cannot create the struct class\n");
    goto r_class;
  }
 
  /*Creating device*/
  if(IS_ERR(device_create(dev_class,NULL,dev,NULL,"raspi_gpio_device"))){
    pr_err( "Cannot create the Device \n");
    goto r_device;
  }
  
  //Checking the GPIO is valid or not
  if(gpio_is_valid(GPIO_19) == false){
    pr_err("GPIO %d is not valid\n", GPIO_19);
    goto r_device;
  }
  
  //Requesting the GPIO
  if(gpio_request(GPIO_19,"GPIO_19") < 0){
    pr_err("ERROR: GPIO %d request\n", GPIO_19);
    goto r_gpio;
  }
  
  //configure the GPIO as output
  gpio_direction_output(GPIO_19, 0);
  
  /* Using this call the GPIO 19 will be visible in /sys/class/gpio/
  ** Now you can change the gpio values by using below commands also.
  ** echo 1 > /sys/class/gpio/gpio19/value  (turn ON the LED)
  ** echo 0 > /sys/class/gpio/gpio19/value  (turn OFF the LED)
  ** cat /sys/class/gpio/gpio19/value  (read the value LED)
  ** 
  ** the second argument prevents the direction from being changed.
  */
  gpio_export(GPIO_19, false);
  
  pr_info("Device Driver Insert...Done!!!\n");
  return 0;
 
r_gpio:
  gpio_free(GPIO_19);
r_device:
  device_destroy(dev_class,dev);
r_class:
  class_destroy(dev_class);
r_del:
  cdev_del(&raspi_gpio_cdev);
r_unreg:
  unregister_chrdev_region(dev,1);
  
  return -1;
}

/*
** Module exit function
*/ 
static void __exit raspi_gpio_driver_exit(void)
{
  gpio_unexport(GPIO_19);
  gpio_free(GPIO_19);
  device_destroy(dev_class,dev);
  class_destroy(dev_class);
  cdev_del(&raspi_gpio_cdev);
  unregister_chrdev_region(dev, 1);
  pr_info("Device Driver Remove...Done!!\n");
}
 
module_init(raspi_gpio_driver_init);
module_exit(raspi_gpio_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Laura Shcherbak <laura.shcherbak@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.0");

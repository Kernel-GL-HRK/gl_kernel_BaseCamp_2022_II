#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/thermal.h>      // Thermal Zone  Temp
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/timer.h>
#include <linux/gpio.h>         //GPIO
#include <linux/err.h>


#define PROC_BUFFER_SIZE 100
#define PROC_DIR_NAME "hello"
#define PROC_FILE_NAME "dummy"

// Buffer and size for the proc file
static char     procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t   procfs_buffer_size = 0;

// Thermal zone device for temperature readings
struct thermal_zone_device *tz;
static int temp;

// GPIO pins for the LEDs
#define GPIO_5 5            // RED 
#define GPIO_6 6            // YELLOW
#define GPIO_26 26          // GREEN

struct gpio led_array[] = {
    { GPIO_5, GPIOF_OUT_INIT_HIGH, "LED_5" },
    { GPIO_6, GPIOF_OUT_INIT_HIGH, "LED_6" },
    { GPIO_26, GPIOF_OUT_INIT_HIGH, "LED_26" }
};

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

static ssize_t hello_read(struct file *file, char __user *buf, size_t count, loff_t *pos)
{
    // Get the temperature from the thermal zone
    if (thermal_zone_get_temp(tz, &temp)) {
        pr_err("Failed to get temperature\n");
    }

    // Determine the size of the buffer to be read
    procfs_buffer_size = min(count, (size_t)PROC_BUFFER_SIZE);                            
    
    // If the current position is greater than or equal to the buffer size, there's nothing more to read
    if (*pos >= procfs_buffer_size) {
        return 0;
    }

    // Format the temperature string to be written to the buffer
    sprintf(procfs_buffer, "CPU temperature = %d.%d Grad\n", temp/1000, temp%1000);

    // If reading the buffer would cause a buffer overflow, adjust the size of the buffer accordingly
    if (*pos + procfs_buffer_size > PROC_BUFFER_SIZE) {
        procfs_buffer_size = PROC_BUFFER_SIZE - *pos;
    }
    
    // Copy the buffer contents to user space
    if (copy_to_user(buf, procfs_buffer + *pos, procfs_buffer_size)) {      
        return -EFAULT;
    }

    // Update the current position in the buffer
    *pos += procfs_buffer_size;

    // Return the number of bytes read
    return procfs_buffer_size;
}

// Create procfs file for temperature
static const struct proc_ops hello_fops = {
    .proc_read = hello_read,
};

// Module Initialization function
static int __init hello_init(void)
{
    // Create proc directory
    proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
    if (!proc_folder) {
        pr_err("Failed to create /proc/%s\n", PROC_DIR_NAME);
        return -ENOMEM;
    }

    // Create proc file
    proc_file = proc_create(PROC_FILE_NAME, 0444, proc_folder, &hello_fops);
    if (!proc_file) {
        pr_err("Failed to create /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
        proc_remove(proc_folder);
        return -ENOMEM;
    }

    //LED Driver Registration
    if (gpio_request_array(led_array, ARRAY_SIZE(led_array))) {
        pr_err("Failed to request GPIO array\n");
        return -1;
    }

    if (thermal_zone_get_temp(tz, &temp)) {
        pr_err("Failed to get temperature\n");
    } else {
        pr_info("CPU Temperature: %d\n", temp / 1000);
    }

    pr_info("Hello Module Inserted /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);

    return 0;
}

static void __exit hello_exit(void)
{
    // LED Driver De-registration
    gpio_free_array(led_array, ARRAY_SIZE(led_array));
    // Remove Proc File
    proc_remove(proc_file);
    proc_remove(proc_folder);
    pr_info("Hello Module Removed /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("V. M.");
MODULE_DESCRIPTION("A");


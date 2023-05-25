# gl_kernel_BaseCamp_2022_II
Linux Kernel Base Camp Autumn 2022


Task06 - is an implementation of a character device driver including procfs and sysfs as well.
Use insmod to load this driver.
The module creates "mymod' file in /dev, which support open/close and read/write file operations. The default size of the driver buffer is 4096 bytes. You can change this size by using ioct().
ioctl provides 3 control operations (see helper macros in mymod_uapi.h):

MYMOD_GET BUFFER_SIZE - writes the buffer size in "arg".
MOD SET_BUFFER_SIZE - resizes buffer to the passed "arg".
MYMOD_CLEAR_BUFFER - clears buffer, setting the data amount (dev_buf_amount) to zero.
The module creates entry "mymod/mymod" in procfs. The file output is a statistical data - buffer size, number of reads/writes, amount of transferred data via read/write callbacks. The sysfs file "/sys/kernel/mymod_cdev/mymod" return this information as well, including the head of buffer.
main.c is a simple program to test ioctl().

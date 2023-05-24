#### Simple GPIO LED Character Device Driver

Character Device Driver

- Has simple sysfs interface
- Has simple ioctl interface

We can blink the LED through the certain GPIO with specified delay.
Delay can be changed with echoing to sysfs file descriptor or with ioctl()
system call from user application

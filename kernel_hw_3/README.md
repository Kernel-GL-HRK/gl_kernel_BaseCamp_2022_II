This is character device driver with following features:
* Add statistical information to `/sys/kernel/chrdev_sysfs/` such as:
    - write call counter represent as `chrdev_write_cnt` file
    - read call counter represent as `chrdev_read_cnt` file
    - buffer, that prints the contents of the buffer, represent as `chrdev_buff` file
    - buffer length, represent as `chrdev_buff_len` file
* Add ability to print content of the buffer using procfs `cat /proc/chrdev`
* Add ioctl interface to
    - clear buffer using `CHRDEV_IOCCBUFF` command
    - get current buffer size using `CHRDEV_IOCGBUFF_LEN` command
    - set buffer size using `CHRDEV_IOCSBUFF_LEN` command

Character device appears in dev as `/dev/chrdev0`
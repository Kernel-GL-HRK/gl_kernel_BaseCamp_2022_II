#ifndef __CHRDEV_IOCTL_H__
#define __CHRDEV_IOCTL_H__

/* ioctl */
#define CDEV_IOC_MAGIC 'V'

enum cdev_ioctl
{
    WR_VALUE,
    RD_VALUE,
    CLEAR_BUFFER,
    GET_BUFFER_SIZE,
    SET_BUFFER_SIZE,
    CDEV_IOC_MAXNR, /* keep last */
};

typedef struct
{
    unsigned int ioctl_to_user_value;
    unsigned int ioctl_from_user_value;
    unsigned int ioctl_arr_size_val;
    unsigned int ioctl_str_size_val;
    unsigned char ioctl_data_buffer[1024];
} data_buffer_info_t;

#define CDEV_WR_VALUE        _IOW(CDEV_IOC_MAGIC, WR_VALUE, data_buffer_info_t *)
#define CDEV_RD_VALUE        _IOR(CDEV_IOC_MAGIC, RD_VALUE, data_buffer_info_t *)
#define CDEV_CLEAR_BUFFER    _IO(CDEV_IOC_MAGIC, CLEAR_BUFFER)
#define CDEV_GET_BUFFER_SIZE _IOR(CDEV_IOC_MAGIC, GET_BUFFER_SIZE, data_buffer_info_t *)
#define CDEV_WRRD_ARR_VALUE  _IOWR(CDEV_IOC_MAGIC, SET_BUFFER_SIZE, data_buffer_info_t *)

#define CDEV_FILE "/dev/chrdev0"

#endif /* __CHRDEV_IOCTL_H__ */
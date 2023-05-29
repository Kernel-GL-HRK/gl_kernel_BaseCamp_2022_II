#ifndef __PROCFS_C__
#define __PROCFS_C__

#include "procfs.h"
#include "globals.h"

static ssize_t proc_rotary_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
    pr_info("%s: read from file %s\n", DEVICE_NAME, filep->f_path.dentry->d_iname);
    pr_info("%s: read from device %d:%d (major:minor)\n", DEVICE_NAME, imajor(filep->f_inode), iminor(filep->f_inode));

    if (len > data_size)
    {
        len = data_size;
    }

    if (copy_to_user(buffer, data_buffer, len))
    {
        pr_err("%s: copy_to_user failed\n", DEVICE_NAME);
        return -EIO;
    }

    data_size = 0;
    pr_info("%s: %zu bytes read\n", DEVICE_NAME, len);

    read_counts++;
    read_amount += len;
    return len;
}
#endif

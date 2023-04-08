#include <linux/string.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/const.h>
#include "proc_ops.h"

MODULE_LICENSE("GPL");

ssize_t procFS_read(struct file *filep, char __user *buffer, size_t count, loff_t *offset)
{
    ssize_t to_copy = 0;
    to_copy = min(count, sizeof(PROC_BUFFER));
	if(*offset != sizeof(PROC_BUFFER)){
    	if(copy_to_user(buffer, PROC_BUFFER, to_copy) != 0)
			return -ENOSPC;
		else{ 
			*offset = to_copy;
			return *offset;
		}
	}
	else
		return 0;
}

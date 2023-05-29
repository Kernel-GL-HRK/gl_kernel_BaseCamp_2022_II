#ifndef __PROCFS_H__
#define __PROCFS_H__

#include "globals.h"
#include "libs.h"

#define PROC_BUFFER_SIZE 500
#define PROC_FILE_NAME "RotaryProcFile" // имя файла в proc директории
#define PROC_DIR_NAME "RotaryProcDir"  // имя proc директории

static int data_size;
static unsigned char data_buffer[BUFFER_SIZE]; // буффер для procfs

struct kobject *chrdev_kobj; 
static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;

static struct proc_dir_entry *proc_file, *proc_folder;

static dev_t dev;
static int delayTime = 600;

static ssize_t proc_rotary_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset);

// функционал procfs
static struct proc_ops proc_fops =
{
    .proc_read = proc_rotary_read
};


#endif
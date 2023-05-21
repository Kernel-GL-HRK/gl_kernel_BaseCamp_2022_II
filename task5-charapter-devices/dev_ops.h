#include <linux/string.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h>

//MAX SIZE OF CHARDEV AND PROC BUFFERS
#define MAX_BUFFER 1024
//SYS AND DEV
#define CLASS_NAME "Keypad"
#define DEVICE_NAME "Matrix"
//TO SELECT THE OUTPUT DIRECTION OF CALCULATION RESULTS
#define FIRST_NOTICE	"----------------------------------------------------------------------\n"\
						"Calculator status messages will be displayed in - ?\n"\
						"To change calculator output direction, write in /dev/Matrix file:\n"\
						"\t1 - dmesg (output in syslog file);\n"\
						"\t2 - /dev/Matrix (output in device file);\n"\
						"\t3 - dmesg and /dev/Matrix.\n"\
						"----------------------------------------------------------------------\n"
#define C1 "dmesg"
#define C2 "/dev/Matrix"
#define C3 "dmesg and /dev/Matrix"
//STATES OF OUTPUT IN DEV FILE
#define READ_FIRST_MSG	0
#define READ_MSG		1
#define READ_LAST_MSG	2
#define READ_END		4

extern uint8_t buffer_chrdev[MAX_BUFFER];
extern uint32_t dev_write_count;
extern uint32_t dev_read_count;


extern struct class *class_tree;
extern struct device *device_file;
extern struct cdev device;
extern struct file_operations dev_fops;
extern dev_t MM;



int dev_open(struct inode *inodep, struct file *filep);

int dev_close(struct inode *inodep, struct file *filep);

ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset);

ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset);

long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg);

void output_choice_message(void);

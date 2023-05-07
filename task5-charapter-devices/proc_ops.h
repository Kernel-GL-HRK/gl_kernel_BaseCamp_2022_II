#include <linux/string.h>
#include <linux/err.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/types.h>
#include <linux/uaccess.h>


//PROC
#define PROC_DIR_NAME "Keypad_Matrix"
#define PROC_FILE_NAME "Driver_Read"
//THE OUTPUT IN A PROC FILE
#define PROC_BUFFER "Matrix keys:\n"\
					"|-----------------|\n"\
					"| [1] [2] [3] [A] |\n"\
					"| [4] [5] [6] [B] |\n"\
					"| [7] [8] [9] [C] |\n"\
					"| [*] [0] [#] [D] |\n"\
					"|-----------------|\n"\
					"Number keys(0-9) responsible for numerics.\n"\
					"\"A\" - (+) add;\n"\
					"\"B\" - (-) subtract;\n"\
					"\"C\" - (*) multiply;\n"\
					"\"D\" - (/) divide;\n"\
					"\"*\" - (<-) clean a symbol\n"\
					"\"#\" - (=) equal and finish calculation\n"

extern struct proc_dir_entry *proc_file;
extern struct proc_dir_entry *proc_dir;
extern struct proc_ops proc_fops;

ssize_t procFS_read(struct file *filep, char __user *buffer, size_t count, loff_t *offset);


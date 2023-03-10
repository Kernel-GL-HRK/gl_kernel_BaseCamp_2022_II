#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
//#include <linux/time.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/err.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kriz");
MODULE_DESCRIPTION("GPIO driver with proc (read) and simple canculator (sum, sub and mul)");
MODULE_VERSION("0.1");

#define PROC_BUFFER_SIZE  100
#define PROC_FILE_NAME   "dummy"
#define PROC_DIR_NAME    "hello"
#define TIMEOUT           5000
#define GPIO_21           (21)

static char   procfs_buffer[PROC_BUFFER_SIZE] = {0};
static size_t procfs_buffer_size              =  0;
static unsigned int count                     =  0;

static struct timer_list      etx_timer;
static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;


void timer_callback(struct timer_list * data)
{
	pr_info("HELLO: TIMER: %s() %d times\n", __func__, count++);
	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}


static ssize_t hello_read(struct file *File, char __user* buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;
	
	if(0 == procfs_buffer_size)
	{
		return 0;
	}
	
	to_copy = min(count, procfs_buffer_size);

	not_copied = copy_to_user(buffer, procfs_buffer, to_copy);

	delta = to_copy - not_copied;
	procfs_buffer_size =- delta;

	return delta;
}

/*static ssize_t hello_write(struct file *file, const char __user* buffer, size_t  count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;
	
	to_copy = min(count, PROC_BUFFER_SIZE);
	
	not_copied = copy_from_user(procfs_buffer, buffer, to_copy);

	delta = to_copy - not_copied;

	procfs_buffer_size = delta;

	return delta;
}*/

static struct proc_ops fops = {
	.proc_read = hello_read
	//.proc_write = hello_write
};

static int op1 = -1;
module_param(op1, int, 00644);
MODULE_PARM_DESC(op1, "First operator for calculation");

static int op2 = -1;
module_param(op2, int, 00644);
MODULE_PARM_DESC(op2, "Second operator for calculation");

static int __init hello_init(void) 
{
	if(gpio_is_valid(GPIO_21) == false)
	{
		pr_err("GPIO %d is not valid\n", GPIO_21);
		goto exit;
	}
	
	if(gpio_request(GPIO_21, "GPIO_21") < 0)
	{
		//-EINVAL;
		pr_err("ERROR: GPIO %d request\n", GPIO_21);
		goto exit;
	}
	
	gpio_direction_output(GPIO_21, 0);
	gpio_set_value(GPIO_21, 1);
	msleep(300);
	gpio_set_value(GPIO_21, 0);
	msleep(100);
	gpio_set_value(GPIO_21, 1);
	msleep(100);
	gpio_set_value(GPIO_21, 0);
	msleep(300);

	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if(!proc_folder)
	{
		pr_err("HELLO: Error: COULD not create /proc/%s/ folder\n", PROC_DIR_NAME);
		return -ENOMEM;
	}

	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &fops);
	if(!proc_file)
	{
		pr_err("HELLO: Error: Could not initialize /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_file);
		proc_remove(proc_folder);
		return -ENOMEM;
	}

	timer_setup(&etx_timer, timer_callback, 0);
	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
	
	pr_info("HELLO: TIMER: %s()\n", __func__);
	pr_info("HELLO: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);
	pr_info("HELLO: Device driver inserted!\nCalculations:\n");
	pr_info("HELLO: Sum: %d + %d = %d\n", op1, op2, op1 + op2);
	pr_info("HELLO: Sub: %d - %d = %d\n", op1, op2, op1 - op2);
	pr_info("HELLO: Mul: %d * %d = %d\n", op1, op2, op1 * op2);
	return 0;
	
	exit:
		return -1;
}

static void __exit hello_exit(void)
{
	gpio_set_value(GPIO_21, 0);
	gpio_free(GPIO_21);
	del_timer(&etx_timer);
	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("HELLO: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
	pr_info("HELLO: TIMER: %s()\n", __func__);
	pr_info("HELLO: Device driver removed!\n");
}

module_init(hello_init);
module_exit(hello_exit);

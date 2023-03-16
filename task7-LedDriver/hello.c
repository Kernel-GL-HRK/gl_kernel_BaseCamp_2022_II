#include <linux/init.h> // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/kernel.h> // Contains types, macros, functions for the kernel
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <linux/err.h>

MODULE_LICENSE("GPL"); ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Maksym Khomenko"); ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Simple gpio driver with proc_fs support"); ///< The description -- see modinfo
MODULE_VERSION("0.2"); ///< The version of the module

#define GPIO_21 (21)		// LED connected to this gpio
#define GPIO_4 (4)	// Button connected to this gpio
#define TIMEOUT 500    // 2000 ms
#define BUTTON_SCAN_PERIOD 100 // 100 ms
#define PROC_BUFFER_SIZE 256

static char procfs_buffer[PROC_BUFFER_SIZE] = "MODULE NAME: my_module\nAUTHOR: Maksym Khomenko\nDESCRIPTION: Simple gpio driver with proc_fs support\nVERSION: 0.2\nLED STATE: ON\n";
static size_t procfs_buffer_size = PROC_BUFFER_SIZE;
static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

static struct timer_list etx_timer;		// LED blink timer
static struct timer_list b_timer;		// Button scan timer

static u8 blink = 1;					// LED state (1 - blink, 0 - not blink)

#define PROC_FILE_NAME "my_info"
#define PROC_DIR_NAME "my_module" /* /proc/my_module/my_state */

static ssize_t my_module_read(struct file *File, char __user* buffer, size_t count, loff_t *offset)
{
	ssize_t to_copy, not_copied, delta;

	if (procfs_buffer_size == 0) 
		{
			procfs_buffer_size = strlen(procfs_buffer);
			return 0;
		}

	to_copy = min(count, procfs_buffer_size);
	not_copied = copy_to_user(buffer, procfs_buffer, to_copy);
	delta = to_copy - not_copied;
	procfs_buffer_size -= delta;
	return delta;
}

// LED blink timer callback
void timer_callback(struct timer_list *data)
{
	char *position_ptr = strstr(procfs_buffer, "LED STATE: ");
	pr_info("MY_MODULE: Timer %s()\n", __func__);
	if (blink)
	{
		if (gpio_get_value(GPIO_21))
		{
			gpio_set_value(GPIO_21, 0);
			strcpy((position_ptr + 11), "OFF\n");
		}
		else
		{
			gpio_set_value(GPIO_21, 1);
			strcpy((position_ptr + 11), "ON\n");
		}
	}
	mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

// Button scan timer callback
void b_timer_callback(struct timer_list *data)
{
	static u8 b_prev_state = 1;
	u8 b_curr_state = gpio_get_value(GPIO_4);
	if (!b_curr_state  && b_prev_state)
	{
		blink = !blink;
	}
	b_prev_state = b_curr_state;
	mod_timer(&b_timer, jiffies + msecs_to_jiffies(BUTTON_SCAN_PERIOD));
}

//static struct file_operations fops = {.read = my_module_read};
static struct proc_ops fops = {.proc_read = my_module_read};


static int __init my_module_init(void)
{
	if (gpio_is_valid(GPIO_21) == false)
	{
		pr_err("MY_MODULE: GPIO %d is not valid\n", GPIO_21);
		return -1;
	}
	if (gpio_is_valid(GPIO_4) == false)
	{
		pr_err("MY_MODULE: GPIO %d is not valid\n", GPIO_4);
		return -1;
	}

	if (gpio_request(GPIO_21, "GPIO_21") < 0)
	{
		pr_err("MY_MODULE: Error GPIO %d request\n", GPIO_21);
		return -1;
	}
	if (gpio_request(GPIO_4, "GPIO_4") < 0)
	{
		pr_err("MY_MODULE: Error GPIO %d request\n", GPIO_4);
		return -1;
	}

	gpio_direction_output(GPIO_21, 0);
	gpio_direction_input(GPIO_4);

	pr_info("MY_MODULE: GPIO %d init done\n", GPIO_21);
	pr_info("MY_MODULE: GPIO %d init done\n", GPIO_4);
	gpio_set_value(GPIO_21, 1);

	proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
	if(!proc_folder)
	{
		pr_err("MY_MODULE: Error Could not create /proc/%s/ folder\n", PROC_DIR_NAME);
		gpio_free(GPIO_21);
		gpio_free(GPIO_4);
		return -ENOMEM;
	}
	proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &fops);
	if(!proc_file)
	{
		pr_err("MY_MODULE: Error Could not initialize /proc/%s/%s\n", PROC_DIR_NAME, PROC_FILE_NAME);
		proc_remove(proc_file);
		proc_remove(proc_folder);
		gpio_free(GPIO_21);
		gpio_free(GPIO_4);
		return -ENOMEM;
	}
procfs_buffer_size = strlen(procfs_buffer);
pr_info("MY_MODULE: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);
timer_setup(&etx_timer, timer_callback, 0);
timer_setup(&b_timer, b_timer_callback, 0);
mod_timer(&etx_timer, jiffies + msecs_to_jiffies(TIMEOUT));
mod_timer(&b_timer, jiffies + msecs_to_jiffies(BUTTON_SCAN_PERIOD));
pr_info("MY_MODULE: Timer %s()\n", __func__);
return 0;
}

static void __exit my_module_exit(void)
{
	del_timer(&etx_timer);
	del_timer(&b_timer);
	pr_info("MY_MODULE: Timer %s() deleted\n", __func__ );
	gpio_set_value(GPIO_21, 0);
	gpio_free(GPIO_21);
	gpio_free(GPIO_4);
	pr_info("MY_MODULE: GPIO free done\n");
	proc_remove(proc_file);
	proc_remove(proc_folder);
	pr_info("MY_MODULE: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
}

module_init(my_module_init);
module_exit(my_module_exit);
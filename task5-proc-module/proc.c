#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include "proc.h"
#include "gpio.h"

static struct proc_dir_entry *proc_timeout;
static struct proc_dir_entry *proc_gpio_status;
static struct proc_dir_entry *proc_folder;

int proc_init(void)
{
	proc_folder = proc_mkdir(PROC_DIR, NULL);
	if (!proc_folder) {
		pr_err("Could not create /proc/%s/ folder\n", PROC_DIR);
		return -ENOMEM;
	}

	proc_gpio_status = proc_create(PROC_GPIO_STATUS, 0666, proc_folder, &gpio_status_ops);
	if (!proc_file) {
		pr_err("Could not initialize /proc/%s/%s\n", PROC_DIR, PROC_GPIO_STATUS);
		proc_exit();
		return -ENOMEM;
	}

	proc_timeout = proc_create(PROC_TIMEOUT, 0666, proc_folder, &timeout_ops);
	if (!proc_file) {
		pr_err("Could not initialize /proc/%s/%s\n", PROC_DIR, PROC_TIMEOUT);
		proc_exit();
		return -ENOMEM;
	}

	return 0;
}

void proc_exit(void)
{
	proc_remove(proc_timeout);
	proc_remove(proc_gpio_status);
	proc_remove(proc_folder);
}

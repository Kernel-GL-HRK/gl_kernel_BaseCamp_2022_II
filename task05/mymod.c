#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

#define DEVICE_NAME "mymod"
#define PROC_NAME "mymod"
#define PERMS 0644

static struct proc_dir_entry *proc_dir, *proc_file;

static struct proc_ops pops = {
	//.read = mymod_read,
	//.write = mymod_write
};

static int __init init_function(void)
{

	pr_info("%s: %s\n", DEVICE_NAME, "INIT");
	proc_dir = proc_mkdir(PROC_NAME, NULL);
        if (IS_ERR(proc_dir))
		goto init_err;
	proc_file = proc_create(PROC_NAME, PERMS, proc_dir, &pops);
	if (IS_ERR(proc_file)) {
		proc_remove(proc_file); 
		goto init_err;
	}
	pr_info("%s: %s\n", DEVICE_NAME, "Success");
	return 0;
        init_err:
               	proc_remove(proc_dir);
		pr_err("%s: %s\n", DEVICE_NAME, "ENOMEM");
		return -ENOMEM;

}

static void __exit exit_function(void)
{
	proc_remove(proc_file);
	proc_remove(proc_dir);
	pr_info("%s: %s\n", DEVICE_NAME, "EXIT");
}


module_init(init_function);
module_exit(exit_function);

MODULE_AUTHOR("Yurii Klysko");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simle kernel module.");

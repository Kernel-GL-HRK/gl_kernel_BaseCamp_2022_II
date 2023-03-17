#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Muravka");
MODULE_DESCRIPTION("Simple program which print \"Hello World\" into syslog and /proc/Calculator/add_sub_mul file");
MODULE_VERSION("2.0");

#define BUFFER_SIZE 500

static struct proc_dir_entry *proc_file;
static struct proc_dir_entry *proc_folder;

#define PROC_FILE_NAME "add_sub_mul"
#define PROC_DIR_NAME "Calculator"

int num1 = 0, num2 = 0;
module_param(num1, int, 0644);
module_param(num2, int, 0644);
static char proc_buffer[BUFFER_SIZE] = {0};

static ssize_t hello_read(struct file *file, char __user *buffer, size_t count, loff_t *offset)
{
  ssize_t to_copy = 0, copied = 0;

  switch (*offset) {
  case 0:
    to_copy = snprintf(proc_buffer, sizeof(proc_buffer), "HELLO: param1{%d} + param2{%d} = %d\n", num1, num2, num1 + num2);
    break;
  case 1:
    to_copy = snprintf(proc_buffer, sizeof(proc_buffer), "HELLO: param1{%d} - param2{%d} = %d\n", num1, num2, num1 - num2);
    break;
  case 2:
    to_copy = snprintf(proc_buffer, sizeof(proc_buffer), "HELLO: param1{%d} * param2{%d} = %d\n", num1, num2, num1 * num2);
    break;
  default:
    return 0;
  }
  to_copy = min(to_copy, count);

  if (copy_to_user(buffer, proc_buffer, to_copy) != 0)
    return -EFAULT;

  *offset += 1;
  copied += to_copy;

  return copied;
}

static struct proc_ops fops = {
    .proc_read = hello_read,
};

static int hello_init(void)
{
  pr_info("HELLO: !!!Hello World!!!\n");

  proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
  if (!proc_folder) {
    pr_err("HELLO: Error: Could not initialize /proc/%s\n", PROC_DIR_NAME);
    return -ENOMEM;
  }
  proc_file = proc_create(PROC_FILE_NAME, 0444, proc_folder, &fops);
  if (!proc_file) {
    pr_err("HELLO: Error: Could not initialize /proc/%s\n", PROC_FILE_NAME);
    return -ENOMEM;
  }
  pr_info("HELLO: /proc/%s/%s created\n", PROC_DIR_NAME, PROC_FILE_NAME);

  if (!(num1 == 0 && num2 == 0)) {
    pr_info("HELLO: param1{%d} + param2{%d} = %d\n", num1, num2, num1 + num2);
    pr_info("HELLO: param1{%d} - param2{%d} = %d\n", num1, num2, num1 - num2);
    pr_info("HELLO: param1{%d} * param2{%d} = %d\n", num1, num2, (num1 * num2));
  }
  return 0;
}

static void hello_exit(void)
{
  proc_remove(proc_file);
  proc_remove(proc_folder);
  pr_info("HELLO: /proc/%s/%s removed\n", PROC_DIR_NAME, PROC_FILE_NAME);
  pr_info("HELLO: Bye!!!");
}

module_init(hello_init);
module_exit(hello_exit);
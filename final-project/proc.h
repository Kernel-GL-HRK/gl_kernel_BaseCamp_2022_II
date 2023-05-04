#include <linux/proc_fs.h>
#include <linux/types.h>

#define PROC_FILE_NAME "FP-servo"
#define ONLY_READ_MODE 0444

extern struct proc_dir_entry *proc_file;
extern struct proc_ops proc_fops;

int32_t create_procFS(void);
void remove_procFS(void);

ssize_t proc_read (struct file *filep, char *to_user, size_t len, loff_t *offs);
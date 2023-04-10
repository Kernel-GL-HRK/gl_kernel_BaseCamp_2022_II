#include <linux/module.h>
#include <linux/init.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include "dev_ops.h"

ssize_t sysfs_show (struct kobject *obj, struct kobj_attribute *attr, char *buf)
{
	ssize_t count;
	count = sprintf(buf, "Call read() for devFS was starded: %d times\n"
							"Call write() for devFS was started: %d times\n"
							"Size of buffer for devFS = %d\n", dev_read_count, dev_write_count, MAX_BUFFER);
	return count;
}
ssize_t sysfs_store (struct kobject *obj, struct kobj_attribute *attr, const char *buf, size_t count){return count;};
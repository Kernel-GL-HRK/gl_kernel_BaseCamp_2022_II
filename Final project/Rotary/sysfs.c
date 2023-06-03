#ifndef __SYSFS_C__
#define __SYSFS_C__

#include "sysfs.h"

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    printk(KERN_INFO "%s: %s\n", DEVICE_NAME, "sysfs - Reading");
    return sprintf(buf, "---Additional info---\nDevice name: %s\nBuffer size: %d\ndev_read() calls: %lld\nCustom user value: %d\n", DEVICE_NAME, BUFFER_SIZE, read_counts, sysfs_val);
}
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count)
{
    printk(KERN_INFO "%s: %s\n", DEVICE_NAME, "sysfs - Writing");
    sscanf(buf, "%d", &sysfs_val);
    return count;
}

#endif

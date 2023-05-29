#ifndef __SYSFS_H__
#define __SYSFS_H__

unsigned int sysfs_val = 0;
static uint64_t read_counts;
static uint64_t read_amount;
struct kobj_attribute chrdev_attr = __ATTR(chrdev_value, 0660, sysfs_show, sysfs_store);

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);

#endif
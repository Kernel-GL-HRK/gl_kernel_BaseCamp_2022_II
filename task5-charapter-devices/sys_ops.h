#include <linux/module.h>
#include <linux/init.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/string.h>

#define FOLDER_SYSFS	"Dev_Ops_Matrix"
#define FILE_SYSFS		"State"

extern struct kobject *Dstate;
extern struct kobj_attribute Fstate;

ssize_t sysfs_show (struct kobject *obj, struct kobj_attribute *attr, char *buf);
ssize_t sysfs_store (struct kobject *obj, struct kobj_attribute *attr,const char *buf, size_t count);
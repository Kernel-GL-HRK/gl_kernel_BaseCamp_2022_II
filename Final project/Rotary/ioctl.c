#ifndef __IOCTL_C__
#define __IOCTL_C__

#include "ioctl.h"
#include "globals.h"

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int err = 0;

    pr_info("%s: %s() cmd=0x%x arg=0x%lx\n", DEVICE_NAME, __func__, cmd, arg);

    if (_IOC_TYPE(cmd) != CDEV_IOC_MAGIC) // если тип cmd совпадает с magic-символом
    {
        pr_err("%s: %s\n", DEVICE_NAME, "CDEV_IOC_MAGIC err");
        return -ENOTTY;
    }
    if (_IOC_NR(cmd) >= CDEV_IOC_MAXNR) // если номер команды больше или равно максимальному числу в enum
    {
        pr_err("%s: %s\n", DEVICE_NAME, "CDEV_IOC_MAXNR err");
        return -ENOTTY;
    }
    if (_IOC_DIR(cmd) & _IOC_READ) // если cmd direction is read и  память доступна
    {
        err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
    }

    if (_IOC_DIR(cmd) & _IOC_WRITE) // если cmd direction is write и память доступна
    {
        err = !access_ok((void __user *)arg, _IOC_SIZE(cmd));
    }

    if (err) // если была ошибка
    {
        pr_err("%s: %s\n", DEVICE_NAME, "_IOC_READ/_IOC_WRITE err");
        return -EFAULT;
    }
    switch (cmd) // выполняем запрошенную команду
    {
    case CDEV_WR_VALUE:
        if (copy_from_user(&ioctl_val, (int32_t *)arg, sizeof(ioctl_val)))
        {
            pr_err("%s: %s\n", DEVICE_NAME, "data write: err");
        }
        pr_info("%s: %s %d\n", DEVICE_NAME, "WR_VALUE ioctl_val =", ioctl_val);
        break;
    case CDEV_RD_VALUE:
        if (copy_to_user((int32_t *)arg, &ioctl_val, sizeof(ioctl_val)))
        {
            pr_err("%s: %s\n", DEVICE_NAME, "data read: err");
        }
        pr_info("%s: %s %d\n", DEVICE_NAME, "RD_VALUE ioctl_val =", ioctl_val);
        break;
    default:
        pr_warn("%s: %s(%u)\n", DEVICE_NAME, "invalid cmd", cmd);
        return -EFAULT;
        break;
    }

    return 0;
}

#endif

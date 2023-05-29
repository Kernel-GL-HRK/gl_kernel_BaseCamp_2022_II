#include <linux/kernel.h>       // для выполнения работы в ядре
#include <linux/module.h>       // для создания модуля. Содержит функции и определения для динамической загрузки модулей ядра
#include <linux/init.h>         // указывает на функции инициализации и очистки 
#include <linux/fs.h>           // содержит функции регистрации и удаления драйвера 
#include <linux/cdev.h>         // содержит необходимые функции для символьного драйвера 
#include <linux/gpio.h>         // для работы с пинами
#include <linux/interrupt.h>    // для работы с прерываниями
#include <linux/uaccess.h>      // предоставляет доступ к пространству пользователя 
#include <linux/kthread.h>      // предоставляет функции для создания и управления ядром потоков
#include <linux/delay.h>        // функции для задержки выполнения
#include <linux/kobject.h>      // функции и макросы для работы с kobject
#include <linux/proc_fs.h>      // для работы с procfs
#include <linux/device.h>       // структуры и функции, связанные с устройствами в ядре
#include <linux/kdev_t.h>       // типы данных, связанные с идентификаторами устройств
#include <linux/err.h>          // для обработки ошибок
#include <linux/ioctl.h>        // для работы с ioctl

#define CLASS_NAME "rotary"
#define DEVICE_NAME "rotary_encoder" // имя файла устройства
#define CDEV_FILE "/dev/rotary1"

#define BUFFER_SIZE 16      // размер буффера для выводимой информации
#define MAX_BUFFER_SIZE 16
#define PERMS 0644          // права доступа
#define MINOR_COUNT 1       // минорный номер для создаваемого устройства
#define UPDATE_INTERVAL 1   // количество секунд между updates to the user-space process

// подключенные GPIO пины
#define GPIO_PIN_A 17       // dt
#define GPIO_PIN_B 18       // clk
#define GPIO_PIN_SW 27      // sw


// переменные положения энкодера
static int prevA = -1;
static int prevB = -1;
static int currA = -1;
static int currB = -1;
static int prevAB = -1;
static int currAB = -1;
static int currBTN = -1;
static int prevBTN = -1;
static int BTNprescount = 0;
static int irq_a, irq_b, irq_sw;
static volatile int encoder_pos = 0;
static struct task_struct *task;

static int is_open; // флаг открытия файла энкодера

#include "globals.h"
#include "libs.h"

//procfs
#define PROC_BUFFER_SIZE 500
#define PROC_FILE_NAME "RotaryProcFile" // имя файла в proc директории
#define PROC_DIR_NAME "RotaryProcDir"  // имя proc директории

static int data_size;
static unsigned char data_buffer[BUFFER_SIZE]; // буффер для procfs

struct kobject *chrdev_kobj; 
static struct class *pclass;
static struct device *pdev;
static struct cdev chrdev_cdev;

static struct proc_dir_entry *proc_file, *proc_folder;

static dev_t dev;
static int delayTime = 600;

static ssize_t proc_rotary_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset);

// функционал procfs
static struct proc_ops proc_fops =
{
    .proc_read = proc_rotary_read
};

//sysfs
unsigned int sysfs_val = 0;
static uint64_t read_counts;
static uint64_t read_amount;
struct kobj_attribute chrdev_attr = __ATTR(chrdev_value, 0660, sysfs_show, sysfs_store);

static ssize_t sysfs_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count);

//ioctl
#define CDEV_IOC_MAGIC 'V'

static int32_t ioctl_val;

enum cdev_ioctl 
{
    WR_VALUE,
    RD_VALUE,
    CDEV_IOC_MAXNR, /* keep last */
};

#define CDEV_WR_VALUE        _IOW(CDEV_IOC_MAGIC, WR_VALUE, int32_t*)
#define CDEV_RD_VALUE        _IOR(CDEV_IOC_MAGIC, RD_VALUE, int32_t*)

static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kriz");
MODULE_DESCRIPTION("Rotary Encoder Driver");

// периодическая таска
static int rotary_encoder_task(void *data)
{
    while (!kthread_should_stop())
    {
        msleep(UPDATE_INTERVAL);//приостанавливаем выполнение потока на UPDATE_INTERVAL миллисекунд
    }
    return 0;
}

// чтение из procfs 
static ssize_t proc_rotary_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
    pr_info("%s: read from file %s\n", DEVICE_NAME, filep->f_path.dentry->d_iname);
    pr_info("%s: read from device %d:%d (major:minor)\n", DEVICE_NAME, imajor(filep->f_inode), iminor(filep->f_inode));

    if (len > data_size)
    {
        len = data_size;
    }

    if (copy_to_user(buffer, data_buffer, len))
    {
        pr_err("%s: copy_to_user failed\n", DEVICE_NAME);
        return -EIO;
    }

    data_size = 0;
    pr_info("%s: %zu bytes read\n", DEVICE_NAME, len);

    read_counts++;
    read_amount += len;
    return len;
}

// чтение и запись в переменную в sysfs
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

// чтение и запись в переменную в ioctl
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

//открытие файла энкодера
static int rotary_encoder_open(struct inode *inode, struct file *filp)
{
    if (is_open)
    {
        pr_err("%s: already open\n", DEVICE_NAME);
        return -EBUSY;
    }
    is_open = 1;

    // Запускаем периодическую таску
    task = kthread_run(rotary_encoder_task, NULL, "rotary_encoder_task");
    pr_info("%s: %s\n", DEVICE_NAME, "open");
    return 0;
}

//закрытие файла энкодера
static int rotary_encoder_release(struct inode *inode, struct file *filp)
{
    // Stop the periodic task
    kthread_stop(task);
    is_open = 0;
    pr_info("%s: %s\n", DEVICE_NAME, "release");
    return 0;
}

//чтение файла энкодера
static ssize_t rotary_encoder_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    // конвертируем позицию энкодера в строку
    char output[BUFFER_SIZE];
    snprintf(output, BUFFER_SIZE, "%d\n", encoder_pos / 2);

    // копируем строку в user-space буффер
    if (copy_to_user(buf, output, strlen(output)) != 0)
    {
        return -EFAULT;
    }

    return strlen(output);
}

// функционал драйвера устройства
static struct file_operations rotary_fops =
{
    .owner = THIS_MODULE,
    .open = rotary_encoder_open,
    .read = rotary_encoder_read,
    .release = rotary_encoder_release,
    .unlocked_ioctl = dev_ioctl,
};

// обработка прерываний
static irqreturn_t rotary_encoder_isr(int irq, void *dev_id)
{
    ndelay(delayTime); // защита от дребезга контактов

    if (irq == irq_sw) // если прерывание от свича
    {
        currBTN = gpio_get_value(GPIO_PIN_SW); // считываем состояние свича
        if (currBTN == prevBTN)                // сравниваем с предыдущим состоянием
        {
            return IRQ_HANDLED;
        }
        else
        {
            BTNprescount++;
            pr_info("%s: %s %d\n", DEVICE_NAME, "Button pressed! BTNprescount is", BTNprescount / 2);
            prevBTN = currBTN;
        }
    }
    if (irq == irq_a || irq == irq_b) // если прерывание от поворота энкодера
    {
        currA = gpio_get_value(GPIO_PIN_A); // считываем состояние контактов
        currB = gpio_get_value(GPIO_PIN_B);

        if (currA == prevA && currB == prevB) // сравниваем с предыдущим положением
        {
            return IRQ_HANDLED;
        }

        if (prevA == 0 && prevB == 0) // определяем номер предыдущего положения энкодера
        {
            prevAB = 0;
        }
        if (prevA == 0 && prevB == 1)
        {
            prevAB = 1;
        }
        if (prevA == 1 && prevB == 1)
        {
            prevAB = 2;
        }
        if (prevA == 1 && prevB == 0)
        {
            prevAB = 3;
        }

        if (currA == 0 && currB == 0) // определяем номер нынешнего положения энкодера
        {
            currAB = 0;
        }
        if (currA == 0 && currB == 1)
        {
            currAB = 1;
        }
        if (currA == 1 && currB == 1)
        {
            currAB = 2;
        }
        if (currA == 1 && currB == 0)
        {
            currAB = 3;
        }

        switch (prevAB) // в зависимости от предыдущего положения энкодера
        {
        case 0:
            if (currAB == 3) // енкодер был повернут влево
            {
                encoder_pos--;
            }
            if (currAB == 1) // енкодер был повернут вправо
            {
                encoder_pos++;
            }
            break;
        case 1:
            if (currAB == 0)
            {
                encoder_pos--;
            }
            if (currAB == 2)
            {
                encoder_pos++;
            }
            break;
        case 2:
            if (currAB == 1)
            {
                encoder_pos--;
            }
            if (currAB == 3)
            {
                encoder_pos++;
            }
            break;
        case 3:
            if (currAB == 2)
            {
                encoder_pos--;
            }
            if (currAB == 0)
            {
                encoder_pos++;
            }
            break;
        }

        pr_info("%s: %s %d\n", DEVICE_NAME, "pos is", encoder_pos / 2);

        prevA = currA; // обновляем положение энкодера
        prevB = currB;
    }

    return IRQ_HANDLED;
}

// Функция для описания прав файла devFS
static int access_to_dev(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}

// init модуля
static int rotary_encoder_init(void)
{
    int ret;
    is_open = 0;
    data_size = 0;

    // определение мажора и минора
    if (alloc_chrdev_region(&dev, 3, 1, DEVICE_NAME))
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Failed to alloc_chrdev_region");
        return -1;
    }

    // добавляем устройство в систему
    cdev_init(&chrdev_cdev, &rotary_fops);
    if ((cdev_add(&chrdev_cdev, dev, 1)) < 0)
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Failed to add the device to the system");
        goto cdev_err;
    }

    // создаем класс устройства
    pclass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(pclass))
    {
        goto class_err;
    }
    pclass->dev_uevent = access_to_dev; // функция для доступа к /dev/*

    // создаем ноду устройства
    pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME "1");
    if (IS_ERR(pdev))
    {
        goto device_err;
    }

    // создаем procfs директорию и файл
    proc_folder = proc_mkdir(PROC_DIR_NAME, NULL);
    if (IS_ERR(proc_folder))
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Error: COULD not create proc_fs folder");
        goto procfs_folder_err;
    }

    proc_file = proc_create(PROC_FILE_NAME, 0666, proc_folder, &proc_fops);
    if (IS_ERR(proc_file))
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Error: Could not initialize proc_fs file");
        goto procfs_file_err;
    }

    // создаем sysfs файл
    chrdev_kobj = kobject_create_and_add("chrdev_sysfs", kernel_kobj);
    if (sysfs_create_file(chrdev_kobj, &chrdev_attr.attr))
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Cannot create sysfs file\n");
        goto sysfs_err;
    }

    // Запрос на доступность GPIO пинов и их резервирование
    gpio_request(GPIO_PIN_A, "rotary_encoder_a");
    gpio_request(GPIO_PIN_B, "rotary_encoder_b");
    gpio_request(GPIO_PIN_SW, "rotary_encoder_sw");

    // Устанавливаем GPIO пины на режим приема сигнала
    gpio_direction_input(GPIO_PIN_A);
    gpio_direction_input(GPIO_PIN_B);
    gpio_direction_input(GPIO_PIN_SW);

    // Устанавливаем значение напряжения на пинах
    gpio_set_value(GPIO_PIN_A, 1);
    gpio_set_value(GPIO_PIN_B, 1);
    gpio_set_value(GPIO_PIN_SW, 1);

    // Получение номера прерывания для пинов
    irq_a = gpio_to_irq(GPIO_PIN_A);
    irq_b = gpio_to_irq(GPIO_PIN_B);
    irq_sw = gpio_to_irq(GPIO_PIN_SW);

    // Запрос и регистрация обработчика прерывания для каждого из номеров прерывания
    ret = request_irq(irq_a, rotary_encoder_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "rotary_encoder_a", NULL);
    if (ret != 0)
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Failed to request IRQ for rotary encoder A");
        goto fail_request_irq;
    }

    ret = request_irq(irq_b, rotary_encoder_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "rotary_encoder_b", NULL);
    if (ret != 0)
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Failed to request IRQ for rotary encoder B");
        goto fail_request_irq;
    }

    ret = request_irq(irq_sw, rotary_encoder_isr, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "rotary_encoder_sw", NULL);
    if (ret != 0)
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Failed to request IRQ for rotary encoder switch");
        goto fail_request_irq;
    }

    // Инициализация была проведена успешно
    pr_info("%s: %s\n", DEVICE_NAME, "INIT");
    pr_info("%s: %s %s\n", DEVICE_NAME, "Device registered as", CLASS_NAME);
    return 0;

fail_request_irq:
    free_irq(irq_a, NULL);
    free_irq(irq_b, NULL);
    free_irq(irq_sw, NULL);
    gpio_free(GPIO_PIN_A);
    gpio_free(GPIO_PIN_B);
    gpio_free(GPIO_PIN_SW);
sysfs_err:
    sysfs_remove_file(kernel_kobj, &chrdev_attr.attr);
    kobject_put(chrdev_kobj);
procfs_file_err:
    proc_remove(proc_file);
procfs_folder_err:
    proc_remove(proc_folder);
device_err:
    class_destroy(pclass);
class_err:
    cdev_del(&chrdev_cdev);
cdev_err:
    unregister_chrdev_region(dev, 1);
    return ret;
}

// exit модуля
static void __exit rotary_encoder_exit(void)
{
    free_irq(irq_a, NULL);
    free_irq(irq_b, NULL);
    free_irq(irq_sw, NULL);
    gpio_free(GPIO_PIN_A);
    gpio_free(GPIO_PIN_B);
    gpio_free(GPIO_PIN_SW);
    sysfs_remove_file(kernel_kobj, &chrdev_attr.attr);
    kobject_put(chrdev_kobj);
    proc_remove(proc_file);
    proc_remove(proc_folder);
    device_destroy(pclass, dev);
    class_destroy(pclass);
    cdev_del(&chrdev_cdev);
    unregister_chrdev_region(dev, MINOR_COUNT);
    pr_info("%s: %s\n", DEVICE_NAME, "EXIT");
}

module_init(rotary_encoder_init);
module_exit(rotary_encoder_exit);

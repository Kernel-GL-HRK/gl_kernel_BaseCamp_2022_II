#include "libs.h"
#include "globals.h"
#include "procfs.h"
#include "sysfs.h"
#include "ioctl.h"
#include "procfs.c"
#include "sysfs.c"
#include "ioctl.c"


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kriz");
MODULE_DESCRIPTION("Rotary Encoder Driver");

// periodic task
static int rotary_encoder_task(void *data)
{
    while (!kthread_should_stop())
    {
        msleep(UPDATE_INTERVAL);
    }
    return 0;
}

//open encoder file
static int rotary_encoder_open(struct inode *inode, struct file *filp)
{
    if (is_open)
    {
        pr_err("%s: already open\n", DEVICE_NAME);
        return -EBUSY;
    }
    is_open = 1;

    // start periodic task
    task = kthread_run(rotary_encoder_task, NULL, "rotary_encoder_task");
    pr_info("%s: %s\n", DEVICE_NAME, "open");
    return 0;
}

//release encoder file
static int rotary_encoder_release(struct inode *inode, struct file *filp)
{
    // stop periodic task
    kthread_stop(task);
    is_open = 0;
    pr_info("%s: %s\n", DEVICE_NAME, "release");
    return 0;
}

//read encoder file
static ssize_t rotary_encoder_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
    // convert the encoder position value into a string
    char output[BUFFER_SIZE];
    snprintf(output, BUFFER_SIZE, "%d\n", encoder_pos / 2);

    // copy the string into the user-space buffer
    if (copy_to_user(buf, output, strlen(output)) != 0)
    {
        return -EFAULT;
    }

    return strlen(output);
}

// functionality of device driver
static struct file_operations rotary_fops =
{
    .owner = THIS_MODULE,
    .open = rotary_encoder_open,
    .read = rotary_encoder_read,
    .release = rotary_encoder_release,
    .unlocked_ioctl = dev_ioctl,
};

// interrupt handling
static irqreturn_t rotary_encoder_isr(int irq, void *dev_id)
{
    ndelay(delayTime); // contact bouncing protection

    if (irq == irq_sw) // if interrupted by the switch
    {
        currBTN = gpio_get_value(GPIO_PIN_SW); // read the status of the switch
        if (currBTN == prevBTN)                
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
    if (irq == irq_a || irq == irq_b) // if interrupted by an encoder rotation
    {
        currA = gpio_get_value(GPIO_PIN_A); // read the status of the a and b
        currB = gpio_get_value(GPIO_PIN_B);

        if (currA == prevA && currB == prevB) // compare to previous
        {
            return IRQ_HANDLED;
        }

        if (prevA == 0 && prevB == 0) // determine the number of the previous encoder position
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

        if (currA == 0 && currB == 0) // determine the number of the current encoder position
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

        switch (prevAB) // depending on the previous encoder position
        {
        case 0:
            if (currAB == 3) // was turned to the left
            {
                encoder_pos--;
            }
            if (currAB == 1) // was turned to the right
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

        prevA = currA; // upd previous state
        prevB = currB;
    }

    return IRQ_HANDLED;
}

// devFS file permissions
static int access_to_dev(struct device *dev, struct kobj_uevent_env *env)
{
    add_uevent_var(env, "DEVMODE=%#o", 0666);
    return 0;
}


static int rotary_encoder_init(void)
{
    int ret;
    is_open = 0;
    data_size = 0;

    // major and minor
    if (alloc_chrdev_region(&dev, 3, 1, DEVICE_NAME))
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Failed to alloc_chrdev_region");
        return -1;
    }

    // add device to the system
    cdev_init(&chrdev_cdev, &rotary_fops);
    if ((cdev_add(&chrdev_cdev, dev, 1)) < 0)
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Failed to add the device to the system");
        goto cdev_err;
    }

    // create dev class
    pclass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(pclass))
    {
        goto class_err;
    }
    pclass->dev_uevent = access_to_dev; // perm to dev file

    // create node
    pdev = device_create(pclass, NULL, dev, NULL, CLASS_NAME "1");
    if (IS_ERR(pdev))
    {
        goto device_err;
    }

    // create procfs entry
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

    // sysfs entry
    chrdev_kobj = kobject_create_and_add("chrdev_sysfs", kernel_kobj);
    if (sysfs_create_file(chrdev_kobj, &chrdev_attr.attr))
    {
        pr_err("%s: %s\n", DEVICE_NAME, "Cannot create sysfs file\n");
        goto sysfs_err;
    }

     
    gpio_request(GPIO_PIN_A, "rotary_encoder_a");
    gpio_request(GPIO_PIN_B, "rotary_encoder_b");
    gpio_request(GPIO_PIN_SW, "rotary_encoder_sw");

    
    gpio_direction_input(GPIO_PIN_A);
    gpio_direction_input(GPIO_PIN_B);
    gpio_direction_input(GPIO_PIN_SW);

    
    gpio_set_value(GPIO_PIN_A, 1);
    gpio_set_value(GPIO_PIN_B, 1);
    gpio_set_value(GPIO_PIN_SW, 1);

    
    irq_a = gpio_to_irq(GPIO_PIN_A);
    irq_b = gpio_to_irq(GPIO_PIN_B);
    irq_sw = gpio_to_irq(GPIO_PIN_SW);

    
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

    // Success
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

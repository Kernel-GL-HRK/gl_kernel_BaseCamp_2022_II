#include <linux/string.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include "proc_ops.h"
#include "keypad.h"
#include "dev_ops.h"
#include "ioctl_cmd.h"

MODULE_LICENSE("GPL");

uint8_t buffer_chrdev[MAX_BUFFER] = {0};
uint32_t dev_write_count = 0;
uint32_t dev_read_count = 0;

// Charapter file operations
int dev_open(struct inode *inodep, struct file *filep)
{
	pr_info("matrix-chrdev: file /dev/%s is opened\n", DEVICE_NAME);
	return 0;
}

int dev_close(struct inode *inodep, struct file *filep)
{
	pr_info("matrix-chrdev: file /dev/%s is closed\n", DEVICE_NAME);
	return 0;
}

ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
	struct calc_s *calc_ptr = get_calc();
	enum calc_out_e calc_out = get_calc_out();
	ssize_t to_copy = sizeof(buffer_chrdev);
	uint16_t end = 0;

	switch (*offset) {
	case READ_FIRST_MSG:
			output_choice_message();
			*offset = READ_MSG;
			dev_read_count++;
			break;
	case READ_MSG:
		if (calc_out == DEV || calc_out == ALL) {
			if ((calc_ptr->operator == '\0') && (calc_ptr->result == 0xFFFFFFFF))
				to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} ? II{%d} = ??          ", calc_ptr->number_01, calc_ptr->number_02);
			else if (calc_ptr->result == 0xFFFFFFFF) {
				switch (calc_ptr->operator) {
				case '+':
					to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} %c II{%d} = %-10d", calc_ptr->number_01, calc_ptr->operator, calc_ptr->number_02, calc_ptr->number_01 + calc_ptr->number_02);
					break;
				case '-':
					to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} %c II{%d} = %-10d", calc_ptr->number_01, calc_ptr->operator, calc_ptr->number_02, calc_ptr->number_01 - calc_ptr->number_02);
					break;
				case '*':
					to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} %c II{%d} = %-10d", calc_ptr->number_01, calc_ptr->operator, calc_ptr->number_02, calc_ptr->number_01 * calc_ptr->number_02);
					break;
				case '/':
					if (calc_ptr->number_02 == 0)
						to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} %c II{%d} = infinity", calc_ptr->number_01, calc_ptr->operator, calc_ptr->number_02);
					else
						to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\rI{%d} %c II{%d} = %-10d", calc_ptr->number_01, calc_ptr->operator, calc_ptr->number_02, calc_ptr->number_01 / calc_ptr->number_02);
					break;
					}
			} else {
				to_copy = snprintf(buffer_chrdev, sizeof(buffer_chrdev), "\n!!!Calculating is finished!!!\n");
				*offset = READ_LAST_MSG;
			}
		} else
			*offset = READ_END;
		break;
	}

	switch (*offset) {
	case READ_LAST_MSG:
		*offset = READ_END;
	case READ_FIRST_MSG:
	case READ_MSG:
		to_copy = min(to_copy, len);
		if (copy_to_user(buffer, buffer_chrdev, to_copy) != 0) {
			pr_err("matrix-chrdev: dont have space to output data into file");
			return -ENOSPC;
		}
		return to_copy;
	case READ_END:
		return 0;
	}
	return 0;
}

ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
	dev_write_count++;

	enum calc_out_e calc_out = get_calc_out();
	uint8_t wr_buffer[MAX_BUFFER] = {0};
	uint32_t i;

	if (copy_from_user(wr_buffer, buffer, len)) {
		pr_err("matrix-chrdev-error: can not read data from user");
		return -ENOSPC;
	}
	pr_info("matrix-chrdev: data is received");
	for (i = 0; i < MAX_BUFFER; i++) {
		switch (wr_buffer[i]) {
		case '1':
			calc_out = DMESG;
			pr_info("matrix-chrdev: calculator output - %s", C1);
			put_calc_out(calc_out);
			return len;
		case '2':
			calc_out = DEV;
			pr_info("matrix-chrdev: calculator output - %s", C2);
			put_calc_out(calc_out);
			return len;
		case '3':
			calc_out = ALL;
			pr_info("matrix-chrdev: calculator output - %s", C3);
			put_calc_out(calc_out);
			return len;
		}
	}
	pr_err("matrix-chrdev: entered wrong choice number, calculator output - %s", C3);
	calc_out = ALL;
	put_calc_out(calc_out);
	return len;
}

long dev_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
	enum calc_out_e calc_out = get_calc_out();

	if (_IOC_TYPE(cmd) != IOCTL_MAGIC) {
		pr_err("matrix-ioctl: program of user space chooses wrong device\n");
		return -ENOKEY;
	}

	switch (_IOC_NR(cmd)) {
	case DMESG:
		pr_info("matrix-ioctl: result output will be desplayed in dmesg\n");
		calc_out = DMESG;
		break;
	case DEV:
		pr_info("matrix-ioctl: result output will be desplayed in dev\n");
		calc_out = DEV;
		break;
	case ALL:
		pr_info("matrix-ioctl: result output will be desplayed in dmesg\n");
		calc_out = ALL;
		break;
	}
	put_calc_out(calc_out);
	return 0;
}

void output_choice_message(void)
{
	enum calc_out_e calc_out = get_calc_out();
	uint16_t i = 0, j = 0, alt = 0;

	while (FIRST_NOTICE[j] != '\0') {
		if (FIRST_NOTICE[j] == '?') {
			switch (calc_out) {
			case DMESG:
				for (alt = 0; C1[alt] != '\0'; alt++)
					buffer_chrdev[i++] = C1[alt];
				break;
			case DEV:
				for (alt = 0; C2[alt] != '\0'; alt++)
					buffer_chrdev[i++] = C2[alt];
				break;
			case ALL:
				for (alt = 0; C3[alt] != '\0'; alt++)
					buffer_chrdev[i++] = C3[alt];
				break;
			}
			j++;
		} else
			buffer_chrdev[i++] = FIRST_NOTICE[j++];
	}
	while (i < sizeof(buffer_chrdev))
		buffer_chrdev[i++] = '\0';
}
// SPDX-License-Identifier: GPL
#include <stdio.h>
#include <sys/types.h>

#include "chardev_ui.h"

int main(void)
{
	int cmd;

	if(cdev_open() < 0) {
		printf ("Open devace failed.\n");
		return -1;
	}

	do {
		printf ("1)\tCear buffer;\n2)\tGet size of buffer;\n3)\tSet size of buffer;\nMake your choice: ");
		scanf ("%d", &cmd);
	} while (cmd < 1 || cmd > 3);

	switch (cmd) {
		case 1:
			cdev_clear();
			printf ("Buffer cleared.\n");
			break;
		case 2:
			int size_buff = cdev_get_size();
			printf ("Size of buffer: %d.\n", size_buff);
			break;
		case 3:
			int size_new = 0;
			printf ("Print new size: ");
			scanf ("%d", &size_new);
			if (size_new < 0)
				size_new = 0;
			cdev_set_size(size_new);
			break;
	}

	cdev_close();

	return 0;
}
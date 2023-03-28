// SPDX-License-Identifier: GPL
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <errno.h>
#include "cdev_ioctl.h"

static const char *const USAGE = (
	"Usage: %s [clr]\n"
	"   or: %s [get]\n"
	"   or: %s [set NUMBER]\n"
);

/**
 * convert_l() - Convert string to long checking errors.
 * @str:	a character string
 * @resptr:	conversion result
 *
 * Return:	true on successful, false otherwise
 */
static bool conv_l(const char *str, long *resptr)
{
#define DECIMAL_BASE	10

	// save errno and zero before conversion
	typeof(errno) _errno = errno;

	errno = 0;
	char *endptr = NULL;
	long conv = strtol(str, &endptr, DECIMAL_BASE);
	bool resgood = (errno != ERANGE) && (str + strlen(str) == endptr);

	if (resgood) {
		*resptr = conv;		// write result only on successful
					// conversion
	}

	errno = _errno;			// restore (probably thread-unsafe)
	return resgood;
}


int main(int argc, char **argv)
{
	if (argc <= 1) {
		fprintf(stderr, USAGE, argv[0], argv[0], argv[0]);
		goto fail;
	}

	int fd, ioctl_good;
	int32_t buf_size, set_size;

	fd = open(CDEV_FILE, O_RDWR);

	if (fd < 0) {
		fprintf(stderr, "Cannot open device file\n");
		goto fail;
	}

	/* ioctl - return 0 or nonnegative value on success.
	 * Each command after its successful execution prints
	 * the size of the buffer.
	 * Setting the new buffer size is done through validation.
	 */
	if (!strcmp(argv[1], "clr") && (argc == 2)) {
		ioctl_good = ioctl(fd, CDEV_CLR_VALUE, (int32_t *) &buf_size);
		if (ioctl_good >= 0) {
			printf("%d\n", buf_size);
		} else {
			fprintf(stderr, "Clear buffer - error\n");
			goto fail;
		}

	} else if (!strcmp(argv[1], "get") && (argc == 2)) {
		ioctl_good = ioctl(fd, CDEV_GET_VALUE, (int32_t *) &buf_size);
		if (ioctl_good >= 0) {
			printf("%d\n", buf_size);
		} else {
			fprintf(stderr, "Get buffer size - error\n");
			goto fail;
		}

	} else if (!strcmp(argv[1], "set") && (argc == 3)) {

		long val;

		if (!conv_l(argv[2], &val)) {
			fprintf(stderr, "Wrong new buffer size\n");
			goto fail;
		}

		if (val > BUFFER_SIZE || val < 0) {
			fprintf(stderr, "Incorect new buffer size\n");
			goto fail;
		}

		set_size = (int32_t)val;
		ioctl_good = ioctl(fd, CDEV_SET_VALUE, (int32_t *) &set_size);
		if (ioctl_good >= 0) {
			printf("%d\n", set_size);
		} else {
			fprintf(stderr, "Set new buffer size - error\n");
			goto fail;
		}

	} else {
		fprintf(stderr, "Wrong command!\n");
		goto fail;
	}

	close(fd);
	return 0;

fail:
	exit(EXIT_FAILURE);
}


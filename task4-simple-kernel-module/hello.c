// SPDX-License-Identifier: GPL-2.0
/*
 * (C) Copyright 2022
 * Author: Roman Kulchytskyi <kulch.roman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; version 2
 * of the License.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Kulchytskyi");
MODULE_DESCRIPTION("Some description");
MODULE_VERSION("0.1");

static int __init hello_init(void)
{
	pr_notice("Hello, world\n");
	return 0;
}

static void __exit hello_exit(void)
{
	pr_notice("Goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);

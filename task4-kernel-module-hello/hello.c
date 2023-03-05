#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Muravka");
MODULE_DESCRIPTION("Simple program which print \"Hello World\" into syslog");
MODULE_VERSION("1.0");

static int hello_init(void){
    pr_info("HELLO: !!!Hello World!!!\n");
    return 0;
}

static void hello_exit(void){
    pr_info("HELLO: Bye!!!");
}

module_init(hello_init);
module_exit(hello_exit);
#include <linux/init.h>         // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>       // Core header for loading LKMs into the kernel
#include <linux/kernel.h>       // Contains types, macros, functions for the kernel


MODULE_LICENSE("GPL");                  ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Yevhen Kolesnyk");       ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("My first module");  ///< The description -- see modinfo
MODULE_VERSION("0.1");                  ///< The version of the module


static int __init hello_init(void){
    pr_info("HELLO: Hello Kernel!\n");
    return 0;
}

static void __exit hello_exit(void){
    pr_info("HELLO: Goodbye Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);
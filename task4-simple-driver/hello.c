#include <linux/init.h> // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h> // Core header for loading LKMs into the kernel
#include <linux/kernel.h> // Contains types, macros, functions for the kernel

MODULE_LICENSE("GPL"); ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Name Surname"); ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Description"); ///< The description -- see modinfo
MODULE_VERSION("0.1"); ///< The version of the module

static int op1 = 0;
static int op2 = 0;

module_param(op1, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(op1, "First parameter of type s32");

module_param(op2, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(op2, "Second parameter of type s32");

static int __init hello_init(void){
pr_info("HELLO: Hello Kernel!\n");
pr_info("HELLO: Sum of parameters = %d\n", op1 + op2);
pr_info("HELLO: Difference of parameters = %d\n", op1 - op2);
pr_info("HELLO: Product of parameters = %d\n", op1 * op2);
return 0;
}

static void __exit hello_exit(void){
pr_info("HELLO: Goodbye Kernel!\n");
}

module_init(hello_init);
module_exit(hello_exit);

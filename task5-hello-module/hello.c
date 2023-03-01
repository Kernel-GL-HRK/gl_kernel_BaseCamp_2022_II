#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>
#include <linux/printk.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kriz");
MODULE_DESCRIPTION("Hello_world module with simple canculator (sum, sub and mul)");
MODULE_VERSION("0.1");

static int op1 = -1;
module_param(op1, int, 00644);
MODULE_PARM_DESC(op1, "First operator for calculation");

static int op2 = -1;
module_param(op2, int, 00644);
MODULE_PARM_DESC(op2, "Second operator for calculation");

static int __init hello_init(void){
pr_info("HELLO: Hello, world!\nCalculations:\n");
pr_info("HELLO:Sum: %d + %d = %d\n", op1, op2, op1 + op2);
pr_info("HELLO:Sub: %d - %d = %d\n", op1, op2, op1 - op2);
pr_info("HELLO:Mul: %d * %d = %d\n", op1, op2, op1 * op2);
return 0;
}

static void __exit hello_exit(void){
pr_info("HELLO: Goodbye, world!\n");
}

module_init(hello_init);
module_exit(hello_exit);

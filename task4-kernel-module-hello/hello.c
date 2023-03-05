#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/moduleparam.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Roman Muravka");
MODULE_DESCRIPTION("Simple program which print \"Hello World\" into syslog");
MODULE_VERSION("1.0");

int num1 = 0, num2 = 0;
module_param(num1, int, 0644);
module_param(num2, int, 0644);

static int hello_init(void){

    pr_info("HELLO: !!!Hello World!!!\n");
    if (!(num1 == 0 && num2 == 0)){
        pr_info("HELLO: param1{%d} + param2{%d} = %d\n", num1, num2, num1 + num2);
        pr_info("HELLO: param1{%d} - param2{%d} = %d\n", num1, num2, num1 - num2);
        pr_info("HELLO: param1{%d} * param2{%d} = %d\n", num1, num2, (num1 * num2));
    }
    return 0;
}

static void hello_exit(void){
    pr_info("HELLO: Bye!!!");
}

module_init(hello_init);
module_exit(hello_exit);
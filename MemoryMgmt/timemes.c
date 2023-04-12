#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <asm/msr.h>
#include <linux/sched.h>

static long size = 1000;
module_param(size, long, 0);
#define CYCLES 1024    //Number of cycles

static int __init init_mod(void)
{
    int i;
    unsigned long order = 1, psize;
    unsigned long long calibr = 0;
    const char *mfun[] = {"kmalloc", "__get_free_pages", "vmalloc"};
    for (psize = PAGE_SIZE; psize < size; order++, psize *= 2) ;
    printk(KERN_INFO "size = %ld order = %ld(%ld)\n", size, order, psize);
    for (i = 0; i < CYCLES; i++)
    {
        cycles_t t1, t2;
        schedule();
        t1 = get_cycles();
        t2 = get_cycles();
        calibr += (t2 - t1);
    }
    calibr = calibr / CYCLES;
    printk(KERN_INFO "calibr=%lld\n", calibr);
    for (i = 0; i < sizeof(mfun) / sizeof(mfun[0]); i++)
    {
        char *kbuf;
        char msg[120];
        int j;
        unsigned long long suma = 0;
        sprintf(msg, "proc. cycles for allocate %s : ", mfun[i]);
        for (j = 0; j < CYCLES; j++)
        {
            unsigned long long t1, t2;
            schedule();
            t1 = get_cycles();
            switch (i)
            {
            case 0:
                kbuf = (char *)kmalloc((size_t)size, GFP_KERNEL);
                break;
            case 1:
                kbuf = (char *)__get_free_pages(GFP_KERNEL, order);
                break;
            case 2:
                kbuf = (char *)vmalloc(size);
            }
            if (!kbuf)
                break;
            t2 = get_cycles();
            suma += (t2 - t1 - calibr);
            switch (i)
            {
            case 0:
                kfree(kbuf);
                break;
            case 1:
                free_pages((unsigned long)kbuf, order);
                break;
            case 2:
                vfree(kbuf);
                break;
            }
        }
        if (kbuf)
            sprintf((msg + strlen(msg)), "%lld", (suma / CYCLES));
        else
            strcat(msg, "failed");
        printk(KERN_INFO "%s\n", msg);
    }
    return -1;
}

static void __exit exit_mod(void)
{
    printk("Mod exit/n");
}
module_init(init_mod);
module_exit(exit_mod);

MODULE_AUTHOR("ME");
MODULE_DESCRIPTION("Memory allocation time measurement");
MODULE_LICENSE( "GPL v2" );
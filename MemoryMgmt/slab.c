#include <linux/module.h>
#include <linux/slab.h>
#include <linux/version.h>

static int size = 9;
module_param(size, int, 0);
static int number = 31;
module_param(number, int, 0);
static void **line = NULL;
static int sco = 0;

static co(void *p)
{
    *(int *)p = (int)p;
    sco++;
}

#define SLABNAME "pos_cache"
struct kmem_cache *cache = NULL;


static int __init init_module(void)
{
    int i;
    if (size < sizeof(void *))
    {
        printk(KERN_ERR "invalid argument\n");
        return -EINVAL;
    }
    line = kmalloc(sizeof(void *) * number, GFP_KERNEL);
    if (!line)
    {
        printk(KERN_ERR "kmalloc error\n");
        goto mout;
    }
    for (i = 0; i < number; i++)
        line[i] = NULL;

    cache = kmem_cache_create(SLABNAME, size, 0, SLAB_HWCACHE_ALIGN, co);
    if (!cache)
    {
        printk(KERN_ERR "kmem_cache_create error\n");
        goto cout;
    }
    for (i = 0; i < number; i++)
    {
        if (NULL == (line[i] = kmem_cache_alloc(cache, GFP_KERNEL)))
        {
            printk(KERN_ERR "kmem_cache_alloc error\n");
            goto oout;
        }
    }
    printk(KERN_INFO "allocate %d objects into slab: %s\n", number, SLABNAME);
    printk(KERN_INFO "object size %d bytes, full size %ld bytes\n", size, (long)size * number);
    printk(KERN_INFO "constructor called %d times\n", sco);
    return 0;
oout:
    for (i = 0; i < number; i++)
    {
        kmem_cache_free(cache, line[i]);
    }
cout:
    kmem_cache_destroy(cache);
mout:
    kfree(line);
    return -ENOMEM;
}

static void __exit exit_module(void)
{
    int i;
    for (i = 0; i < number; i++)
        kmem_cache_free(cache, line[i]);
    kmem_cache_destroy(cache);
    kfree(line);
}
module_init(init_module);
module_exit(exit_module);

MODULE_LICENSE( "GPL" );
MODULE_AUTHOR( "ME" ); 
MODULE_VERSION( "1.21" );
MODULE_DESCRIPTION("SLAB Allocator example");
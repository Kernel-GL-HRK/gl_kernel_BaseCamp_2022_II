#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>

#define MEMPOOL_BLOCK_SIZE 64
#define MEMPOOL_NUM_BLOCKS 1000

struct mempool
{
    void *mempool;
    unsigned int block_size;
    unsigned int num_blocks;
    void **free_blocks;
    unsigned int num_free_blocks;
};

void mempool_init(struct mempool *mp, unsigned int block_size, unsigned int num_blocks)
{
    unsigned int i = 0;
    mp->block_size = block_size;
    mp->num_blocks = num_blocks;
    mp->num_free_blocks = num_blocks;
    mp->mempool = kmalloc(block_size * num_blocks, GFP_KERNEL);
    if (!mp->mempool)
    {
        printk(KERN_ERR "Failed to allocate memory for memory pool\n");
        return;
    }

    mp->free_blocks = kmalloc(num_blocks * sizeof(void *), GFP_KERNEL);
    if (!mp->free_blocks)
    {
        kfree(mp->mempool);
        mp->mempool = NULL;
        printk(KERN_ERR "Failed to allocate memory for memory pool free blocks\n");
        return;
    }

    for (i = 0; i < num_blocks; i++)
    {
        mp->free_blocks[i] = mp->mempool + (i * block_size);
    }
}

void *mempool_alloc(struct mempool *mp)
{
    if (mp->num_free_blocks == 0)
    {
        printk(KERN_ERR "Memory pool is empty\n");
        return NULL;
    }

    void *block = mp->free_blocks[--mp->num_free_blocks];
    mp->free_blocks[mp->num_free_blocks] = NULL;
    return block;
}

void mempool_free(struct mempool *mp, void *block)
{
    if (!block)
    {
        printk(KERN_ERR "Attempting to free a NULL pointer\n");
        return;
    }

    if ((uintptr_t)block < (uintptr_t)mp->mempool || (uintptr_t)block >= (uintptr_t)mp->mempool + (mp->block_size * mp->num_blocks))
    {
        printk(KERN_ERR "Error: the passed memory block is not within the memory pool\n");
        return;
    }

    uintptr_t block_offset = (uintptr_t)block - (uintptr_t)mp->mempool;
    if (block_offset % mp->block_size != 0)
    {
        printk(KERN_ERR "Error: the passed memory block does not start at the beginning of a memory pool block\n");
        return;
    }

    mp->free_blocks[mp->num_free_blocks++] = block;
}

static int __init my_module_init(void)
{
    // Initialize the memory pool with block size 64 bytes and 1000 blocks
    struct mempool mp;
    mempool_init(&mp, MEMPOOL_BLOCK_SIZE, MEMPOOL_NUM_BLOCKS);

    // Allocate memory blocks from the memory pool
    void *block1 = mempool_alloc(&mp);
    void *block2 = mempool_alloc(&mp);
    void *block3 = mempool_alloc(&mp);

    // Print the addresses of the allocated blocks
    printk(KERN_INFO "Address of block 1: %p\n", block1);
    printk(KERN_INFO "Address of block 2: %p\n", block2);
    printk(KERN_INFO "Address of block 3: %p\n", block3);

    // Free the memory blocks back to the memory pool
    mempool_free(&mp, block1);
    mempool_free(&mp, block2);
    mempool_free(&mp, block3);
    // Clean up the memory pool
    kfree(mp.free_blocks);
    kfree(mp.mempool);

    return 0;
}

static void __exit my_module_exit(void)
{
    printk(KERN_INFO "Exiting my_module\n");
}

module_init(my_module_init);
module_exit(my_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Memory Pool Example");

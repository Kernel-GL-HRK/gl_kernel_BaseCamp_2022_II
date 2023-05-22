# Task 07: Create driver for character device

1. Create simple driver for character device:
   + one /dev/file, one buffer. Buffer size ~1k,
   + Handlers: read(), write(), open(), release()
2. Add entry for procfs: only handler for read(): prints symbol buffer content.
3. Add entry for sysfs: only handler for sysfs_read():
   - sysfs handler prints content of the driver symbol buffer.
   - sysfs handler prints :chrdev buffer size
   - sysfs handler prints number of device openings (open(), release())
4. Add ioctl support:
   - CLEAR_BUFFER buffer clear: size=0, ‘\0’ - clear first symbol or memset() of the whole buffer
   - GET_BUFFER_SIZE buffer size get
   - SET_BUFFER_SIZE check if val < BUFFER_SIZE check if buffer size doesn't exceed allocated regionfor buffer (1k)
+ GitHub: module.c module.ko, main.c (userspase app calls ioctl).

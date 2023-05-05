# Task 8. Charapter Devices

Writing driver to support charapter device

## Part_1:
	- Write simple character device driver.
	- Buffer size ~1k,
	- Handlers: read(), write(), open(), release()
	- Proc: read() chrdev buffer print

## Part_2:
	- Add capability to existing character device driver
	- sysfs_read() :chrdev buffer print
		     :chrdev buffer size print
		     :number of dev_read() and dev_write() calls

## Part_3:
	- Add ioctl capability to character device driver
	- ioctl:
		:CLEAR_BUFFER    buffer clear: size=0, ‘\0’
		:GET_BUFFER_SIZE buffer size get
		:SET_BUFFER_SIZE check if val < BUFFER_SIZE
	
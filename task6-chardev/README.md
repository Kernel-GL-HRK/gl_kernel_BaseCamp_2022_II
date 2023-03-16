# Chrdev homework: part 1
Write simple character device driver. <br/>
Buffer size ~1k, <br/>
Handlers: read(), write(), open(), release() <br/>
Proc: read() chrdev buffer print <br/>

# Chrdev homework: part 2
Add capability to existing character device driver <br/>
sysfs_read() <br/>
:chrdev buffer print <br/>
:chrdev buffer size print <br/>
:number of dev_read() and dev_write() calls <br/>

# Chrdev homework: part 3
Add ioctl capability to character device driver <br/>
ioctl: <br/>
:CLEAR_BUFFER    buffer clear: size=0, ‘\0’ <br/>
:GET_BUFFER_SIZE buffer size get <br/>
:SET_BUFFER_SIZE check if val < BUFFER_SIZE <br/>
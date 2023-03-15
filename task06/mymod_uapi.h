#ifndef mymod_uapi_h
#define mymod_uapi_h

#define MYMOD_MAGIC 155

enum mymod_ctl {
	CLEAR_BUFFER = 1,
	GET_BUFFER_SIZE,
	SET_BUFFER_SIZE,
	MAXNR,
};

#define MYMOD_CLEAR_BUFFER _IO(MYMOD_MAGIC, CLEAR_BUFFER)
#define MYMOD_GET_BUFFER_SIZE _IOR(MYMOD_MAGIC, GET_BUFFER_SIZE, uint32_t *)
#define MYMOD_SET_BUFFER_SIZE _IOW(MYMOD_MAGIC, SET_BUFFER_SIZE, uint32_t)

#define DEVICE_NAME "mymod"
#define DEVICE_CLASS "mymod_cdev"

#define MYMOD_FILE "/dev/"DEVICE_NAME

#endif 

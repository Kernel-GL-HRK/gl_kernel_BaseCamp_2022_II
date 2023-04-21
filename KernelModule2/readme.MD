# Home work

● Add proc functionality to Your's hello module (read only handler).
● Create a separate folder and commit on BaseCamp Github repository the next files: 
● hello.c, hello.ko, Makefile
● Deadline: EoD 08.03.2023

# Bash

● Prepare sources
```
git clone --depth 1 --branch v5.3 git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git

make clean
make mrproper
export BUILD_KERNEL=/path/to/kernel/sources
git checkout v5.3 -b work_5_3
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- multi_v7_defconfig
make -j8 ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- zImage
```

● Fix multiple definition of 'yylloc' error
```vi scripts/dtc/dtc-lexer.lex.c
```

● checkpatch.pl
```checkpatch.pl --no-tree -f hello.c
```

Build for ARM architecture
```
make ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- KDIR=../BaseCamp/linux
```

● Working with module in Linux
```shell
sudo insmod hello.ko v1=2 v2=4
sudo lsmod | grep hello
sudo rmmod hello.ko
```

# Results

dmesg

tail -f /var/log/kern.log

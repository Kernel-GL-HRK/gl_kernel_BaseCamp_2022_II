# Home work

● Write hello_world module: fill module author, description, version e.t.c.

● Write into the console
- module initialization;
- module de-initialization;

● Use module parameters to write simple calculator: module should print sum, 
substraction and multiplication of two numbers passed as parameters.

● Don’t forget to use checkpatch.pl – all not-corresponding drivers will be rejected.

● Github: hello.c, hello.ko, Makefile

● Deadline: EoD 23.02.2023

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
```shell
[12262.963197] HELLO: Hello Kernel
[12262.963206] Sum: 2 + 4 = 6
[12262.963209] Substraction: 2 - 4 = -2
[12262.963211] Multiplication: 2 * 4 = 8
[12293.439917] HELLO: Goodbye Kernel
```

tail -f /var/log/kern.log
```shell
Feb 22 05:55:23 laura-VirtualBox kernel: [12262.963197] HELLO: Hello Kernel
Feb 22 05:55:23 laura-VirtualBox kernel: [12262.963206] Sum: 2 + 4 = 6
Feb 22 05:55:23 laura-VirtualBox kernel: [12262.963209] Substraction: 2 - 4 = -2
Feb 22 05:55:23 laura-VirtualBox kernel: [12262.963211] Multiplication: 2 * 4 = 8
Feb 22 05:55:53 laura-VirtualBox kernel: [12293.439917] HELLO: Goodbye Kernel
```

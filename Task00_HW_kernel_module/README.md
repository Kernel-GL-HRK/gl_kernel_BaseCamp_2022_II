# Home work
[x] Write hello_world module: fill module author, description, version e.t.c.

[x] Write into the console

- [x] module initialization;

- [x] module de-initialization;

[x] Use module parameters to write simple calculator: module should print sum,
substraction and multiplication of two numbers passed as parameters.

[x] Don’t forget to use checkpatch.pl – all not-corresponding drivers will be
rejected.

[x] Github: hello.c, hello.ko, Makefile

## Sample commands

```shell
insmod hello.ko
rmmod hello
```

```shell
insmod hello.ko sum=1,2
```

```shell
insmod hello.ko sum=1,2 subst=2,4 prod=3,3
```

dmesg results:

```shell
[  +7,950982] HELLO: Hello Kernel!
[  +0,000001] SUM(1,2) = 3
[  +0,000001] SUBST(2,4) = -2
[  +0,000000] PROD(3,3) = 9
[  +7,678936] HELLO: Goodbye Kernel!
```

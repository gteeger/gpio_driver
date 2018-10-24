export ARCH:=arm
export CROSS_COMPILE:=arm-linux-gnueabihf-
export CC:=arm-linux-gnueabihf-gcc
obj-m := gpio_driver.o
KDIR := /home/gt/Desktop/xcompiled_kernel/ti-linux-kernel-dev/KERNEL
PWD := $(shell pwd)
mod:
		$(MAKE) -C $(KDIR) M=$(PWD) modules

app:
		$(CC) test.c -o test

clean:
		$(MAKE) -C $(KDIR) M=$(PWD) clean
		rm -f test
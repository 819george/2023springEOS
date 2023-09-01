obj-m += lab3-2_driver.o
all:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C /home/george/linux M=$(shell pwd) modules
clean:
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C /home/george/linux M=$(shell pwd) clean

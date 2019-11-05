obj-m += procfs.o


.PHONY: check clean test_read test_ioctl reload create_dev

all:
	make -C /usr/src/linux-headers-$(shell uname -r) M=$(PWD) modules
	~/sign_module.sh procfs.ko

check:
	lsmod | grep procfs

test_read:
	gcc test_read.c -o test_read

test_ioctl:
	gcc test_ioctl.c -o test_ioctl

reload:
	rmmod procfs
	insmod procfs.ko

clean:
	make -C /usr/src/linux-headers-$(shell uname -r) M=$(PWD) clean
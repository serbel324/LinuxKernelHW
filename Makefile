obj-m += phone_book_module.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

test:
	wc sudo rmmod phone_book_device 2>/dev/null
	sudo dmesg -C
	sudo insmod phone_book_device.ko
	sudo dmesg

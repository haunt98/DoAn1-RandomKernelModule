obj-m += rand_char_device.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
test:
	gcc test_rand_char_device.c -o test_rand_char_device.o -Wall

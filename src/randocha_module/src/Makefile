EXTRA_CFLAGS = -Wall

obj-m        += randocha_module.o

KDIR = /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD)
	$(CC) user_space.c -o user_space
clean:
	make -C $(KDIR) M=$(PWD) clean
	rm user_space

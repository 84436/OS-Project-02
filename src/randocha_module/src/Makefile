KDIR = /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=`pwd`
	$(CC) user_space.c -o user_space
clean:
	make -C $(KDIR) M=`pwd` clean
	rm user_space

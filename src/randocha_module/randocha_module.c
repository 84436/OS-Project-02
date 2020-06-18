#include <linux/module.h> // header for loading LKMs
#include <linux/random.h> // for get_random_bytes
#include <linux/init.h>   // for __init __exit
#include <linux/kernel.h> // for default types, macros, functions
#include <linux/types.h>  // dev_t for major n minor number
#include <linux/fs.h>     // system support & logging
#include <linux/uaccess.h>// for copy to user_space function
#include <linux/device.h>


#define MAX (int)1e9
#define DEV_NAME "randocha"
#define CHAR_DEV_NAME "RC"


static int major_number;
static int open_cnt = 0;
static struct class *device_class = NULL;
static struct device *device = NULL;

static int dev_open(struct inode *, struct file *);

static int dev_release(struct inode *, struct file *);

static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

MODULE_LICENSE("GPL");

MODULE_AUTHOR("@t3bol90 @84436 @neokhongmut");


static struct file_operations fops =
        {
                .open = dev_open,
                .read = dev_read,
                .release = dev_release,
        };


static int __init randocha_init(void) {
    major_number = register_chrdev(0, DEV_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "RC: failed to reg major number\n");
        return major_number;
    }

    printk(KERN_INFO "RC: reg successfully, major number is %d\n", major_number);


    device_class = class_create(THIS_MODULE, CHAR_DEV_NAME);

    if (IS_ERR(device_class)) {
        unregister_chrdev(major_number, DEV_NAME);
        printk(KERN_ALERT "RC: failed to reg device class\n");
        return PTR_ERR(device_class);
    }
    printk(KERN_INFO "RC: reg successfully, device class\n");

    device = device_create(device_class, NULL, MKDEV(major_number, 0), NULL, DEV_NAME);
    if (IS_ERR(device)) {
        unregister_chrdev(major_number, DEV_NAME);
        printk(KERN_ALERT "RC: failed to register device driver\n");
        return PTR_ERR(device);
    }
    printk(KERN_INFO "RC: device class created correctly\n");
    return 0;
}


static void __exit randocha_exit(void) {
    // Rules: unregister before destroy
    // device_unregister(device_class);
    device_destroy(device_class, MKDEV(major_number, 0));
    class_unregister(device_class);
    class_destroy(device_class);
    unregister_chrdev(major_number, DEV_NAME);
    printk(KERN_INFO "RC: random character unregistered");
}

static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "RC: device has been opened %d time(s)\n", open_cnt++);
    return 0;
}
static ssize_t dev_read(struct file *filep, char *usr_space, size_t len, loff_t *offset) {
    int _res;
    get_random_bytes(&_res, sizeof(_res));
    _res %= MAX;

    // return 0 if success
    if (!copy_to_user(usr_space, &_res, sizeof(_res))) {
        printk(KERN_INFO "RC: return randomized number to the user\n");
        return 0;
    }
        // else return error: Bad address
    else {
        printk(KERN_INFO "RC: failed to send randomized number to the user\n");
        return -EFAULT;
    }
}
static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "RC: device closed\n");
    return 0;
}



module_init(randocha_init); // plug in
module_exit(randocha_exit); // plug out

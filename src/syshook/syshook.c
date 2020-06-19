// Tested on Ubuntu 14.04.5 (linux-4.4.0-31-generic)

#include <linux/init.h> // module init and exit
#include <linux/module.h> // required for *all* modules
#include <linux/kernel.h> // kernel macros and functions
#include <linux/syscalls.h>
#include <linux/fdtable.h>
#include <linux/slab.h> // kmalloc, kfree
// Module information
MODULE_LICENSE("GPL");

/********************************************************************************/
// SYS_CALL_TABLE-RELATED

static void** SYS_CALL_TABLE = NULL;

// Automagically get sys_call_table location in memory
// Check if the symbol is available (exported) for kernel module use.
// grep -w kallsyms_lookup_name /lib/modules/$(uname -r)/build/Modules.symvers
// void get_sys_call_table(void)
// {
//    SYS_CALL_TABLE = (void**)kallsyms_lookup_name("sys_call_table");
//}

// Alternatively, manually grab and hard-code the address.
// grep -w sys_call_table /boot/System.map-$(uname -r) | cut -d ' ' -f 1

// pte = Page Table Entry

// Make a page writable
int make_rw (unsigned long address) {
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    if (pte->pte &~_PAGE_RW) { pte->pte |= _PAGE_RW; }
    return 0;
}

// Make a page read-only
int make_ro (unsigned long address) {
    unsigned int level;
    pte_t *pte = lookup_address(address, &level);
    pte->pte = pte->pte &~ _PAGE_RW;
    return 0;
}

/********************************************************************************/
// CUSTOM HOOKS

// open_original(), open_custom()
// write_original(), write_custom()

asmlinkage int (*open_original) (const char*, int, int);
asmlinkage int (*write_original) (unsigned int, const char*, int);

asmlinkage int open_custom (const char* file, int flags, int mode) {
    char* buf = kmalloc(256, GFP_KERNEL);
    copy_from_user(buf, file, 256);
    printk(KERN_INFO "[syshook] open() : %s opened %s\n", current->comm, buf);
    kfree(buf);
    // printk(KERN_INFO "[syshook] open() : file = %s\n", file);
    // print_process_name();
    return open_original(file, flags, mode);
}

asmlinkage int write_custom (unsigned int fd, const char* buf, int count) {
    char* fileName = kmalloc(256, GFP_KERNEL);
    char* cwd = d_path(&files_fdtable(current->files)->fd[fd]->f_path, fileName, 256);
    printk(KERN_INFO "[syshook] write() : %s wrote %d byte(s) in %s\n", current->comm, count, cwd);
    kfree(fileName);
    // print_process_name();
    return write_original(fd, buf, count);
}

/********************************************************************************/
// MODULE INIT + EXIT

static int __init hook_init(void) {
    // printk(KERN_INFO "[syshook] ");

    SYS_CALL_TABLE = (void*)0xffffffff818001c0;

    printk(KERN_INFO "[syshook] sys_call_table found at %p\n", SYS_CALL_TABLE);
    open_original = SYS_CALL_TABLE[__NR_open];
    write_original = SYS_CALL_TABLE[__NR_write];
    printk(KERN_INFO "[syshook] original open() and write() backed up\n");
    
    printk(KERN_INFO "[syshook] original open() at %p\n", open_original);
    printk(KERN_INFO "[syshook] original write() at %p\n", write_original);
    printk(KERN_INFO "[syshook] custom open() at %p\n", open_custom);
    printk(KERN_INFO "[syshook] custom write() at %p\n", write_custom);
    
    make_rw((unsigned long)SYS_CALL_TABLE);
    printk(KERN_INFO "[syshook] sys_call_table made r/w\n");
    
    SYS_CALL_TABLE[__NR_open] = open_custom;
    SYS_CALL_TABLE[__NR_write] = write_custom;
    printk(KERN_INFO "[syshook] custom syscalls hooked\n");
    
    return 0;
}

static void __exit hook_cleanup(void){
    SYS_CALL_TABLE[__NR_open] = open_original;
    SYS_CALL_TABLE[__NR_write] = write_original;
    printk(KERN_INFO "[syshook] original syscalls restored\n");
    
    make_ro((unsigned long)SYS_CALL_TABLE);
    printk(KERN_INFO "[syshook] sys_call_table made r/o\n");
}

module_init(hook_init);
module_exit(hook_cleanup);

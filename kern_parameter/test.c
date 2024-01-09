//kernel parameters can be specified from commandline
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

//Try: 
// sudo insmod test.ko myint=55 name="CHANGED"
//Loads module with user defined params else default are specified here

MODULE_LICENSE("GPL");

char *name="TESTING";
int myint=3;

module_param(name, charp,0);
module_param(myint,int,0);

static int __init func_init(void) {
    
    printk(KERN_INFO "Module has been loaded with args: %s %d\n", name,myint);
    return 0;
}

static void __exit func_exit(void) {
    printk(KERN_INFO "Module has been unloaded\n");
}

module_init(func_init);
module_exit(func_exit);
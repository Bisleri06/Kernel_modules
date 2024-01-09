//only one process can open this device at a time
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

//create device using: 
//sudo mknod /dev/<device_name> c <major_number> 0

MODULE_LICENSE("GPL");

#define DEVICE_NAME "blocker"

static int dev_open(struct inode*,struct file*);
static int dev_release(struct inode*,struct file*);
static ssize_t dev_read(struct file*,char *,size_t,loff_t*);
static ssize_t dev_write(struct file*,const char *,size_t,loff_t*);

static struct file_operations fops={
    .open=dev_open,
    .read=dev_read,
    .write=dev_write,
    .release=dev_release,
};

static int major;
static int opened;


static int __init blocker_init(void){
    major=register_chrdev(0,DEVICE_NAME,&fops);

    if(major<0){
        printk(KERN_ALERT "blocker LOAD FAILED\n");
        return major;
    }

    printk(KERN_INFO "blocker loaded %d\n",major);
    return 0;
}


static void __exit blocker_exit(void){
    unregister_chrdev(major,DEVICE_NAME);
    printk(KERN_INFO "blocker ded\n");
}


module_init(blocker_init);
module_exit(blocker_exit);


static int dev_open(struct inode *inodep, struct file *filep) {
   if(opened==1){
    printk(KERN_INFO "blocker device already opened\n");
    return -EBUSY;
   }

   printk(KERN_INFO "blocker device opened\n");
   opened=1;
   return 0;
}

static ssize_t dev_write(struct file *filep, const char *buffer,size_t len, loff_t *offset) {

   printk(KERN_INFO "Sorry, blocker is read only\n");
   return -EFAULT;
}

static int dev_release(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "blocker device closed\n");
   opened=0;
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer,size_t len, loff_t *offset) {
    int errors=0;
    char *msg="TESTTEST";
    int mlen=strlen(msg);

    errors=copy_to_user(buffer,msg,mlen);

    return errors==0?len:-EFAULT;
}
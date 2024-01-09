//device to encrypt and decrypt data
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

//create device using: 
//sudo mknod /dev/<device_name> c <major_number> 0

MODULE_LICENSE("GPL");

#define DEVICE_NAME "encryptor"

//can pass different encryption byte in commandline too, see kern parameters module for example
char *encbyte=0xFC;
module_param(encbyte, charp,0);

static int dev_open(struct inode*, struct file*);
static int dev_release(struct inode*, struct file*);
static ssize_t dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t dev_write(struct file*, const char*, size_t, loff_t*);

static struct file_operations fops = {
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};


static char clip_buff[1024];
static int major;

static int __init encryptor_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);

    if (major < 0) {
        printk(KERN_ALERT "encryptor load failed\n");
        return major;
    }

    printk(KERN_INFO "encryptor module has been loaded: %d\n", major);
    return 0;
}

static void __exit encryptor_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "encryptor module has been unloaded\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "encryptor device opened\n");
   return 0;
}


static ssize_t dev_write(struct file *filep, const char *buffer,size_t len, loff_t *offset) {

    int error;
    error=0;
    memset(clip_buff,0,1024);
    error = copy_from_user(clip_buff,buffer,1024);

    printk(KERN_INFO "%d bytes written\n",1024);
    return error == 0 ? len : -EFAULT;
}



static int dev_release(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "encryptor device closed\n");
   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    
    char temp[1024];
    int i;
    int errors;
    
    memset(temp,0,1024);

    for(i=0;i<1024;i++)
    {
        if(clip_buff[i]==0)
            break;
        temp[i]=clip_buff[i]^encbyte[0];
    }

    errors = 0;
    errors = copy_to_user(buffer, temp, 1024);

    printk(KERN_INFO "%d bytes read\n",1024);
    return errors == 0 ? len : -EFAULT;
}

module_init(encryptor_init);
module_exit(encryptor_exit);
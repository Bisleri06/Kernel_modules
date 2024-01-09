//write password to file and read secret content after which reset happens
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

//sudo insmod vault.ko password="WORKS"
//create device using: 
//sudo mknod /dev/<device_name> c <major_number> 0

MODULE_LICENSE("GPL");

#define DEVICE_NAME "vault"

static char *password="password123";
static int pass_len;
module_param(password, charp, 0);

static char *secret_buffer="SEKRUT TEXT\n";

char pass_buf[100];
static int major;


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



static int __init vault_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);

    if (major < 0) {
        printk(KERN_ALERT "vault load failed\n");
        return major;
    }

    pass_len=strlen(password);

    printk(KERN_INFO "vault module has been loaded: %d and password is %s\n", major,password);
    return 0;
}

static void __exit vault_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "vault module has been unloaded\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "vault device opened\n");
   return 0;
}


static ssize_t dev_write(struct file *filep, const char *buffer,size_t len, loff_t *offset) {

    int error;
    error=0;
    memset(pass_buf,0,100);
    error = copy_from_user(pass_buf,buffer,100);

    printk(KERN_INFO "password written\n");
    return error == 0 ? len : -EFAULT;
}


static int dev_release(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "vault device closed\n");
   return 0;
}


static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    
    int errors;
    int secret_len=strlen(secret_buffer);
    int i;
    
    errors = 0;

    for(i=0;i<pass_len;i++)
    {
        if(pass_buf[i]!=password[i])
            return -EFAULT;
    }
    
    errors = copy_to_user(buffer, secret_buffer, secret_len);
    printk(KERN_INFO "%d bytes read\n",secret_len);
    
    memset(pass_buf,0,100);

    return errors == 0 ? len : -EFAULT;
}

module_init(vault_init);
module_exit(vault_exit);
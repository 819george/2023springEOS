#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/uaccess.h> //copy_to/from_user()

MODULE_LICENSE("GPL");
dev_t dev = 0; 
static struct class *dev_class;  
static struct cdev cdev;    
int flag=0;
int tmp[10]={0};
int seg_for_c[27] = {
    0b1111001100010001, // A
    0b0000011100000101, // b
    0b1100111100000000, // C
    0b0000011001000101, // d
    0b1000011100000001, // E
    0b1000001100000001, // F
    0b1001111100010000, // G
    0b0011001100010001, // H
    0b1100110001000100, // I
    0b1100010001000100, // J
    0b0000000001101100, // K
    0b0000111100000000, // L
    0b0011001110100000, // M
    0b0011001110001000, // N
    0b1111111100000000, // O
    0b1000001101000001, // P
    0b0111000001010000, //q
    0b1110001100011001, //R
    0b1101110100010001, //S
    0b1100000001000100, //T
    0b0011111100000000, //U
    0b0000001100100010, //V
    0b0011001100001010, //W
    0b0000000010101010, //X
    0b0000000010100100, //Y
    0b1100110000100010, //Z
    0b0000000000000000
};

static int my_open(struct inode *inode, struct file *file)
{
 pr_info("Device File Opened...!!!\n");
 return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
 pr_info("Device File Closed...!!!\n");
 return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
 char seg[17];
 int j=0;
 
 for(j=0;j<16;j++){
  seg[j] = ((tmp[flag] >> (15 - j)) & 1) ? '1' : '0';
 }
 if( copy_to_user(buf, &seg, len) > 0) 
  pr_err("ERROR: Not all the bytes have been copied to user\n");
 
 flag++;
 return len;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
 uint8_t rec_buf[10] = {0};
 int i;
 
 if( copy_from_user( rec_buf, buf, len ) > 0) 
  pr_err("ERROR: Not all the bytes have been copied from user\n");

 for(i=0;i<6;i++){
  if(rec_buf[i]>='a' && rec_buf[i]<='z')
   tmp[i]=seg_for_c[rec_buf[i]-'a'];
  else if(rec_buf[i]>='A' && rec_buf[i]<='Z')
   tmp[i]=seg_for_c[rec_buf[i]-'A'];
  else
   pr_err("ERROR: Name must be alphabet\n");
 }
 return len;
}

static struct file_operations fops =
{
 .owner = THIS_MODULE,
 .read = my_read,
 .write = my_write,
 .open = my_open,
 .release = my_release,
};

static int __init my_init(void)
{
 /*Allocating Major number*/
 if((alloc_chrdev_region(&dev, 0, 1, "mydev")) <0){
 pr_err("Cannot allocate major number\n");
 goto r_unreg;
 }
 pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 /*Creating cdev structure*/
 cdev_init(&cdev,&fops);
 /*Adding character device to the system*/
 if((cdev_add(&cdev,dev,1)) < 0){
 pr_err("Cannot add the device to the system\n");
 goto r_del;
 }
 
 /*Creating struct class*/
 if((dev_class = class_create(THIS_MODULE,"myclass")) == NULL){
 pr_err("Cannot create the struct class\n");
 goto r_class;
 }
 /*Creating device*/
 if((device_create(dev_class,NULL,dev,NULL,"mydev")) == NULL){
 pr_err( "Cannot create the Device \n");
 goto r_device;
 }
 pr_info("Device Driver Insert...Done!!!\n");
 return 0;

r_device:
 device_destroy(dev_class,dev);
r_class:
 class_destroy(dev_class);
r_del:
 cdev_del(&cdev);
r_unreg:
 unregister_chrdev_region(dev,1);

 return -1;
}

static void __exit my_exit(void)
{
 device_destroy(dev_class,dev);
 class_destroy(dev_class);
 cdev_del(&cdev);
 unregister_chrdev_region(dev, 1);
 pr_info("Device Driver Remove...Done!!\n");
}
module_init(my_init);
module_exit(my_exit);

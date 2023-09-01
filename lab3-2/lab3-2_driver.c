#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
//#include <syswait.h> // usleep(n) n微秒 // Sleep（n）n毫秒 // sleep（n）n秒
#include <linux/uaccess.h> //copy_to/from_user()
#include <linux/gpio.h> //GPIO
//LED is connected to this GPIO
#define GPIO_21 (21)  //a
#define GPIO_16 (16)  //b
#define GPIO_12 (12)  //c
#define GPIO_1 (1)    //d
#define GPIO_7 (7)    //e
#define GPIO_8 (8)   //f
#define GPIO_25 (25)  //g
dev_t dev = 0;                   // 裝置編號
static struct class *dev_class;  // 裝置群組
static struct cdev etx_cdev;     // 字元裝置設備
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp,
 char __user *buf, size_t len,loff_t * off);    //user space buffer
static ssize_t etx_write(struct file *filp,     //Operations that can be performed on the device
 const char *buf, size_t len, loff_t * off);
/******************************************************/
//File operation structure
// struct file_operations 決定怎麼跟設備檔案溝通的方式
// 在規格中我們希望只透過"寫入"來開關LED，所以只需定義.write
static struct file_operations fops =
{
 .owner = THIS_MODULE,
 .read = etx_read,
 .write = etx_write,
 .open = etx_open,
 .release = etx_release,
};
/*
** This function will be called when we open the Device file
*/
static int etx_open(struct inode *inode, struct file *file)
{
 pr_info("Device File Opened...!!!\n");
 return 0;
}
/*
** This function will be called when we close the Device file
*/
static int etx_release(struct inode *inode, struct file *file)
{
 pr_info("Device File Closed...!!!\n");
 return 0;
}
/*
** This function will be called when we read the Device file
*/
static ssize_t etx_read(struct file *filp,
 char __user *buf, size_t len, loff_t *off)
{
 uint8_t gpio_state = 0;

 //reading GPIO value
 gpio_state = gpio_get_value(GPIO_21);

 //write to user
 len = 1;
 if( copy_to_user(buf, &gpio_state, len) > 0) {
 pr_err("ERROR: Not all the bytes have been copied to user\n");
 }

 pr_info("Read function : GPIO_21 = %d \n", gpio_state);

 return 0;
}
/*
** This function will be called when we write the Device file
*/
static ssize_t etx_write(struct file *filp,
 const char __user *buf, size_t len, loff_t *off)
{
 uint8_t rec_buf[10] = {0};
 int i;
 
 if( copy_from_user( rec_buf, buf, len ) > 0) {
 pr_err("ERROR: Not all the bytes have been copied from user\n");
 }
 
 pr_info("Write Function : GPIO_21 Set = %s\n", rec_buf);
 
 for(i=0;i<10;i++){
  if(rec_buf[i]=='0'){
  gpio_set_value(GPIO_21, 1);
  gpio_set_value(GPIO_16, 1);
  gpio_set_value(GPIO_12, 1);
  gpio_set_value(GPIO_1,  1);
  gpio_set_value(GPIO_7,  1);
  gpio_set_value(GPIO_8,  1);
  gpio_set_value(GPIO_25, 0);
  }
  else if(rec_buf[i]=='1'){
  gpio_set_value(GPIO_21, 0);
  gpio_set_value(GPIO_16, 1);
  gpio_set_value(GPIO_12, 1);
  gpio_set_value(GPIO_1,  0);
  gpio_set_value(GPIO_7,  0);
  gpio_set_value(GPIO_8,  0);
  gpio_set_value(GPIO_25, 0);
  }
  else if(rec_buf[i]=='2'){
  gpio_set_value(GPIO_21, 1);
  gpio_set_value(GPIO_16, 1);
  gpio_set_value(GPIO_12, 0);
  gpio_set_value(GPIO_1,  1);
  gpio_set_value(GPIO_7,  1);
  gpio_set_value(GPIO_8,  0);
  gpio_set_value(GPIO_25, 1);
  }
  else if(rec_buf[i]=='3'){
  gpio_set_value(GPIO_21, 1);
  gpio_set_value(GPIO_16, 1);
  gpio_set_value(GPIO_12, 1);
  gpio_set_value(GPIO_1,  1);
  gpio_set_value(GPIO_7,  0);
  gpio_set_value(GPIO_8,  0);
  gpio_set_value(GPIO_25, 1);
  }
  else if(rec_buf[i]=='4'){
  gpio_set_value(GPIO_21, 0);
  gpio_set_value(GPIO_16, 1);
  gpio_set_value(GPIO_12, 1);
  gpio_set_value(GPIO_1,  0);
  gpio_set_value(GPIO_7,  0);
  gpio_set_value(GPIO_8,  1);
  gpio_set_value(GPIO_25, 1);
  }
  else if(rec_buf[i]=='5'){
  gpio_set_value(GPIO_21, 1);
  gpio_set_value(GPIO_16, 0);
  gpio_set_value(GPIO_12, 1);
  gpio_set_value(GPIO_1,  1);
  gpio_set_value(GPIO_7,  0);
  gpio_set_value(GPIO_8,  1);
  gpio_set_value(GPIO_25, 1);
  }
  else{
  gpio_set_value(GPIO_21, 0);
  gpio_set_value(GPIO_16, 0);
  gpio_set_value(GPIO_12, 0);
  gpio_set_value(GPIO_1,  0);
  gpio_set_value(GPIO_7,  0);
  gpio_set_value(GPIO_8,  0);
  gpio_set_value(GPIO_25, 0);
  }
  mdelay(1000);
 }
 
 return len;
}
/*
** Module Init function
*/
static int __init etx_driver_init(void)
{
 /*Allocating Major number*/
 if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
 pr_err("Cannot allocate major number\n");
 goto r_unreg;
 }
 pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 /*Creating cdev structure*/
 cdev_init(&etx_cdev,&fops);
 /*Adding character device to the system*/
 if((cdev_add(&etx_cdev,dev,1)) < 0){
 pr_err("Cannot add the device to the system\n");
 goto r_del;
 }
 /*Creating struct class*/
 if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
 pr_err("Cannot create the struct class\n");
 goto r_class;
 }
 /*Creating device*/
 if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
 pr_err( "Cannot create the Device \n");
 goto r_device;
 }

 //Checking the GPIO is valid or not
 if(gpio_is_valid(GPIO_21) == false){
 pr_err("GPIO %d is not valid\n", GPIO_21);
 goto r_device;
 }
 if(gpio_is_valid(GPIO_16) == false){
 pr_err("GPIO %d is not valid\n", GPIO_16);
 goto r_device;
 }
 if(gpio_is_valid(GPIO_12) == false){
 pr_err("GPIO %d is not valid\n", GPIO_12);
 goto r_device;
 }
 if(gpio_is_valid(GPIO_1) == false){
 pr_err("GPIO %d is not valid\n", GPIO_1);
 goto r_device;
 }
 if(gpio_is_valid(GPIO_7) == false){
 pr_err("GPIO %d is not valid\n", GPIO_7);
 goto r_device;
 }
 if(gpio_is_valid(GPIO_8) == false){
 pr_err("GPIO %d is not valid\n", GPIO_8);
 goto r_device;
 }
 if(gpio_is_valid(GPIO_25) == false){
 pr_err("GPIO %d is not valid\n", GPIO_25);
 goto r_device;
 }
 //Requesting the GPIO
 if(gpio_request(GPIO_21,"GPIO_21") < 0){
 pr_err("ERROR: GPIO %d request\n", GPIO_21);
 goto r_gpio;
 }
 if(gpio_request(GPIO_16,"GPIO_16") < 0){
 pr_err("ERROR: GPIO %d request\n", GPIO_16);
 goto r_gpio;
 }
 if(gpio_request(GPIO_12,"GPIO_12") < 0){
 pr_err("ERROR: GPIO %d request\n", GPIO_12);
 goto r_gpio;
 }
 if(gpio_request(GPIO_1,"GPIO_1") < 0){
 pr_err("ERROR: GPIO %d request\n", GPIO_1);
 goto r_gpio;
 }
 if(gpio_request(GPIO_7,"GPIO_1") < 0){
 pr_err("ERROR: GPIO %d request\n", GPIO_7);
 goto r_gpio;
 }
 if(gpio_request(GPIO_8,"GPIO_1") < 0){
 pr_err("ERROR: GPIO %d request\n", GPIO_8);
 goto r_gpio;
 }
 if(gpio_request(GPIO_25,"GPIO_1") < 0){
 pr_err("ERROR: GPIO %d request\n", GPIO_25);
 goto r_gpio;
 }

 //configure the GPIO as output
 gpio_direction_output(GPIO_21, 0);
 gpio_direction_output(GPIO_16, 0);
 gpio_direction_output(GPIO_12, 0);
 gpio_direction_output(GPIO_1, 0);
 gpio_direction_output(GPIO_7, 0);
 gpio_direction_output(GPIO_8, 0);
 gpio_direction_output(GPIO_25, 0);
 /* Using this call the GPIO 21 will be visible in /sys/class/gpio/
 ** Now you can change the gpio values by using below commands also.
 ** echo 1 > /sys/class/gpio/gpio21/value (turn ON the LED)
 ** echo 0 > /sys/class/gpio/gpio21/value (turn OFF the LED)
 ** cat /sys/class/gpio/gpio21/value (read the value LED)
 **
 ** the second argument prevents the direction from being changed.
 */
 gpio_export(GPIO_21, false);
 gpio_export(GPIO_16, false);
 gpio_export(GPIO_12, false);
 gpio_export(GPIO_1, false);
 gpio_export(GPIO_7, false);
 gpio_export(GPIO_8, false);
 gpio_export(GPIO_25, false);
 
 pr_info("Device Driver Insert...Done!!!\n");
 return 0;
r_gpio:
 gpio_free(GPIO_21);
 gpio_free(GPIO_16);
 gpio_free(GPIO_12);
 gpio_free(GPIO_1);
 gpio_free(GPIO_7);
 gpio_free(GPIO_8);
 gpio_free(GPIO_25);
r_device:
 device_destroy(dev_class,dev);
r_class:
 class_destroy(dev_class);
r_del:
 cdev_del(&etx_cdev);
r_unreg:
 unregister_chrdev_region(dev,1);

 return -1;
}
/*
** Module exit function
*/
static void __exit etx_driver_exit(void)
{
 gpio_unexport(GPIO_21);
 gpio_free(GPIO_21);
 gpio_unexport(GPIO_16);
 gpio_free(GPIO_16);
 gpio_unexport(GPIO_12);
 gpio_free(GPIO_12);
 gpio_unexport(GPIO_1);
 gpio_free(GPIO_1);
 gpio_unexport(GPIO_7);
 gpio_free(GPIO_7);
 gpio_unexport(GPIO_8);
 gpio_free(GPIO_8);
 gpio_unexport(GPIO_25);
 gpio_free(GPIO_25);
 device_destroy(dev_class,dev);
 class_destroy(dev_class);
 cdev_del(&etx_cdev);
 unregister_chrdev_region(dev, 1);
 pr_info("Device Driver Remove...Done!!\n");
}
module_init(etx_driver_init);
module_exit(etx_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("A simple device driver - GPIO Driver");
MODULE_VERSION("1.32");

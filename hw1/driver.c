#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h> 
#include <linux/uaccess.h> //copy_to/from_user()
#include <linux/gpio.h> //GPIO

#define GPIO_21 (21)  //a
#define GPIO_16 (16)  //b
#define GPIO_12 (12)  //c
#define GPIO_1 (1)    //d
#define GPIO_7 (7)    //e
#define GPIO_8 (8)   //f
#define GPIO_25 (25)  //g

#define GPIO_18 (18)  //p1
#define GPIO_15 (15)  //p2
#define GPIO_14 (14)  //p3

int flag=4;

dev_t dev = 0;                   
static struct class *dev_class;  
static struct cdev etx_cdev;    
static int __init etx_driver_init(void);
static void __exit etx_driver_exit(void);
/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp,
 char __user *buf, size_t len,loff_t * off);    //user space buffer
static ssize_t etx_write(struct file *filp,     
 const char *buf, size_t len, loff_t * off);
void seg(char v);
/******************************************************/
static struct file_operations fops =
{
 .owner = THIS_MODULE,
 .read = etx_read,
 .write = etx_write,
 .open = etx_open,
 .release = etx_release,
};

static int etx_open(struct inode *inode, struct file *file)
{
 pr_info("Device File Opened...!!!\n");
 return 0;
}

static int etx_release(struct inode *inode, struct file *file)
{
 pr_info("Device File Closed...!!!\n");
 return 0;
}

static ssize_t etx_read(struct file *filp,
 char __user *buf, size_t len, loff_t *off)
{
 uint8_t gpio_state = 0;

 gpio_state = gpio_get_value(GPIO_21);

 len = 1;
 if( copy_to_user(buf, &gpio_state, len) > 0) {
 pr_err("ERROR: Not all the bytes have been copied to user\n");
 }

 pr_info("Read function : GPIO_21 = %d \n", gpio_state);

 return 0;
}

void seg(char v){
        if(v=='0'){
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_1,  1);
        gpio_set_value(GPIO_7,  1);
        gpio_set_value(GPIO_8,  1);
        gpio_set_value(GPIO_25, 0);
        }
        else if(v=='1'){
        gpio_set_value(GPIO_21, 0);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_1,  0);
        gpio_set_value(GPIO_7,  0);
        gpio_set_value(GPIO_8,  0);
        gpio_set_value(GPIO_25, 0);
        }
        else if(v=='2'){
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 0);
        gpio_set_value(GPIO_1,  1);
        gpio_set_value(GPIO_7,  1);
        gpio_set_value(GPIO_8,  0);
        gpio_set_value(GPIO_25, 1);
        }
        else if(v=='3'){
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_1,  1);
        gpio_set_value(GPIO_7,  0);
        gpio_set_value(GPIO_8,  0);
        gpio_set_value(GPIO_25, 1);
        }
        else if(v=='4'){
        gpio_set_value(GPIO_21, 0);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_1,  0);
        gpio_set_value(GPIO_7,  0);
        gpio_set_value(GPIO_8,  1);
        gpio_set_value(GPIO_25, 1);
        }
        else if(v=='5'){
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_16, 0);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_1,  1);
        gpio_set_value(GPIO_7,  0);
        gpio_set_value(GPIO_8,  1);
        gpio_set_value(GPIO_25, 1);
        }
        else if(v=='6'){
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_16, 0);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_1,  1);
        gpio_set_value(GPIO_7,  1);
        gpio_set_value(GPIO_8,  1);
        gpio_set_value(GPIO_25, 1);
        }
        else if(v=='7'){
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_1,  0);
        gpio_set_value(GPIO_7,  0);
        gpio_set_value(GPIO_8,  0);
        gpio_set_value(GPIO_25, 0);
        }
        else if(v=='8'){
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_1,  1);
        gpio_set_value(GPIO_7,  1);
        gpio_set_value(GPIO_8,  1);
        gpio_set_value(GPIO_25, 1);
        }
        else if(v=='9'){
        gpio_set_value(GPIO_21, 1);
        gpio_set_value(GPIO_16, 1);
        gpio_set_value(GPIO_12, 1);
        gpio_set_value(GPIO_1,  0);
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
    }

static ssize_t etx_write(struct file *filp,
 const char __user *buf, size_t len, loff_t *off)
{
 uint8_t rec_buf[10] = {0};
 int i=0;

 if( copy_from_user( rec_buf, buf, len ) > 0) {
 pr_err("ERROR: Not all the bytes have been copied from user\n");
 }
 
 pr_info("Write Function : GPIO_21 Set = %s\n", rec_buf);
    

    if(rec_buf[0]=='0'){//whole
        for(i=0;i<2;i++){
            if(rec_buf[1]=='1') gpio_set_value(GPIO_18, 1);
            else gpio_set_value(GPIO_18, 0);
            if(rec_buf[2]=='1') gpio_set_value(GPIO_15, 1);
            else gpio_set_value(GPIO_15, 0);
            if(rec_buf[3]=='1') gpio_set_value(GPIO_14, 1);
            else gpio_set_value(GPIO_14, 0);
            seg(rec_buf[flag]);
            mdelay(500);
            seg('a');
            if(flag==4) flag++;
            else if(flag==5) flag--;
        }
    }
    else if(rec_buf[0]=='1'){//set
        for(i=0;i<3;i++){
            if(rec_buf[1]=='2'){
                gpio_set_value(GPIO_18, 0);
                gpio_set_value(GPIO_15, 0);
                gpio_set_value(GPIO_14, 0);
                seg(rec_buf[4]);
                mdelay(500);
                gpio_set_value(GPIO_18, 1);
                seg(rec_buf[5]);
                mdelay(500);
            }
            if(rec_buf[2]=='2'){
                gpio_set_value(GPIO_18, 0);
                gpio_set_value(GPIO_15, 0);
                gpio_set_value(GPIO_14, 0);
                seg(rec_buf[4]);
                mdelay(500);
                gpio_set_value(GPIO_15, 1);
                seg(rec_buf[5]);
                mdelay(500);
            }
            if(rec_buf[3]=='2'){
                gpio_set_value(GPIO_18, 0);
                gpio_set_value(GPIO_15, 0);
                gpio_set_value(GPIO_14, 0);
                seg(rec_buf[4]);
                mdelay(500);
                gpio_set_value(GPIO_14, 1);
                seg(rec_buf[5]);
                mdelay(500);
            }
                
        }
        //gpio_set_value(GPIO_18, 0);
        //gpio_set_value(GPIO_15, 0);
        //gpio_set_value(GPIO_14, 0);
    }
    else{
        gpio_set_value(GPIO_18, 0);
        gpio_set_value(GPIO_15, 0);
        gpio_set_value(GPIO_14, 0);
        seg(rec_buf[0]);
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
 if(gpio_is_valid(GPIO_18) == false){
 pr_err("GPIO %d is not valid\n", GPIO_18);
 goto r_device;
 }
 if(gpio_is_valid(GPIO_15) == false){
 pr_err("GPIO %d is not valid\n", GPIO_15);
 goto r_device;
 }
 if(gpio_is_valid(GPIO_14) == false){
 pr_err("GPIO %d is not valid\n", GPIO_14);
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
 if(gpio_request(GPIO_18,"GPIO_1") < 0){
 pr_err("ERROR: GPIO %d request\n", GPIO_18);
 goto r_gpio;
 }
 if(gpio_request(GPIO_15,"GPIO_1") < 0){
 pr_err("ERROR: GPIO %d request\n", GPIO_15);
 goto r_gpio;
 }
 if(gpio_request(GPIO_14,"GPIO_1") < 0){
 pr_err("ERROR: GPIO %d request\n", GPIO_14);
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
 gpio_direction_output(GPIO_18, 0);
 gpio_direction_output(GPIO_15, 0);
 gpio_direction_output(GPIO_14, 0);
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
 gpio_export(GPIO_18, false);
 gpio_export(GPIO_15, false);
 gpio_export(GPIO_14, false);

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
 gpio_free(GPIO_18);
 gpio_free(GPIO_15);
 gpio_free(GPIO_14);

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
 gpio_unexport(GPIO_18);
 gpio_free(GPIO_18);
 gpio_unexport(GPIO_15);
 gpio_free(GPIO_15);
 gpio_unexport(GPIO_14);
 gpio_free(GPIO_14);
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

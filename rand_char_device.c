#include <linux/device.h>  // for struct device, class
#include <linux/fs.h>      // for struct file_operations
#include <linux/init.h>    // for macro __init, __exit
#include <linux/kernel.h>  // macro, function for kernel
#include <linux/module.h>  // for loading module
#include <linux/mutex.h>   // for mutex sync
#include <linux/random.h>  // for function get_random_bytes()
#include <linux/uaccess.h> // for function copy_to_user()

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tran Hau");
MODULE_DESCRIPTION("Simple character device to send user random number");

static DEFINE_MUTEX(rand_char_mutex); // only 1 program access at a time

#define DEVICE_NAME "rand_char"
#define CLASS_NAME "rand_char_class"

static int major_number; // device number
static struct class *rand_char_class = NULL;
static struct device *rand_char_device = NULL;

// Prototype
static int rand_char_open(struct inode *, struct file *);
static int rand_char_release(struct inode *, struct file *);
static ssize_t rand_char_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops = {
    .open = rand_char_open,
    .release = rand_char_release,
    .read = rand_char_read,
};

static int __init rand_char_init(void)
{
    printk(KERN_INFO "rand char module init\n");

    // try to allocate major number
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT
               "rand char device failed to register a major number\n");
        return major_number;
    }
    printk(KERN_INFO "rand char device registered a major number %d\n",
           major_number);

    // register class
    rand_char_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(rand_char_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "rand char device failed to create a struct "
                          "pointer class\n");
        return PTR_ERR(rand_char_class); // return error for pointer
    }
    printk(KERN_INFO "rand char device created a struct pointer class\n");

    // register device
    rand_char_device = device_create(rand_char_class, NULL,
                                     MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(rand_char_device)) {
        class_destroy(rand_char_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "rand char device failed to create a struct "
                          "pointer device\n");
        return PTR_ERR(rand_char_device); // return error for pointer
    }
    printk(KERN_INFO "rand char device created a struct pointer device\n");

    // set up mutex
    mutex_init(&rand_char_mutex);

    return 0;
}

static void __exit rand_char_exit(void)
{
    // cleanup mutex
    mutex_destroy(&rand_char_mutex);

    // cleanup device, class, unregister major number
    device_destroy(rand_char_class, MKDEV(major_number, 0));
    class_unregister(rand_char_class);
    class_destroy(rand_char_class);
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "rand char module exit\n");
}

// call each time device is opened
static int rand_char_open(struct inode *inodep, struct file *filep)
{
    // check if another process is opening device
    if (!mutex_trylock(&rand_char_mutex)) {
        printk(KERN_ALERT "rand char device is opened by another process\n");
        return -EBUSY;
    }

    printk(KERN_INFO "rand char device is opened\n");
    return 0;
}

// call each time device is released
static int rand_char_release(struct inode *inodep, struct file *filep)
{
    // unlock mutex
    mutex_unlock(&rand_char_mutex);

    printk(KERN_INFO "rand char device is released\n");
    return 0;
}

// call each time device is read
// device send random number to user
static ssize_t rand_char_read(struct file *filep, char *buffer, size_t len,
                              loff_t *offset)
{
    // random number in kernel space
    int rand_num;
    get_random_bytes(&rand_num, sizeof(rand_num));

    int status = 0;
    // copy_to_user return 0 - success
    // otherwise - fail
    status = copy_to_user(buffer, &rand_num, sizeof(rand_num));
    if (status == 0) {
        printk(KERN_INFO "rand char sent random number %d\n", rand_num);
        return 0;
    } else {
        printk(KERN_INFO "rand char failed to send random number %d\n",
               rand_num);
        return -EFAULT;
    }
}

module_init(rand_char_init);
module_exit(rand_char_exit);

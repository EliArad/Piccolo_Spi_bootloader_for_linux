#include <linux/module.h>    // included for all kernel modules 
#include <linux/kernel.h>    // included for KERN_INFO 
#include <linux/init.h>      // included for __init and __exit macros 
#include <linux/gpio.h> 
#include <linux/kthread.h>  // for threads 
#include <linux/time.h>   // for using jiffies 
#include <linux/timer.h> 
#include <linux/delay.h> 
#include <linux/fs.h>
#include <asm/uaccess.h> /* for put_user */
#include <linux/slab.h>
#include <linux/ioctl.h>
#include "piccolo_spi_bootloader_io.h"

/*
 * Prototypes - this would normally go in a .h file
 */

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static long device_ioctl(struct file *f, unsigned int cmd, unsigned long arg);


unsigned char *pKernelBuffer = NULL;

typedef struct USER_MESSAGE
{
	int index;
	int size;

} USER_MESSAGE;

unsigned char myBuffer[100];

#define SUCCESS 0
#define DEVICE_NAME "piccolobl"
#define BUF_LEN 80

#define MAJOR_DEVICE_NUN 74

static int Device_Open = 0;
static char msg[BUF_LEN];
static char *msg_Ptr;



static struct file_operations fops = { .read = device_read,
										.write = device_write,
										.open = device_open,
										.unlocked_ioctl = device_ioctl,
										.release = device_release };

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Eli Arad");
MODULE_DESCRIPTION("SPI Slave for updating piccolo firmware");
MODULE_VERSION("1.0");

//static char *gpionum = "gpionum"; 
int gpionum = 118;
//module_param(gpionum, int, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
//MODULE_PARM_DESC(gpionum, "SPI Slave for updating piccolo firmware");  ///< parameter description

//static int gpio_led;

static int m_moduleAlive = 0;

static int __init piccolobl_init(void)
{

	int res;
	printk(KERN_INFO "Piccolo bootloader initialized!\n");

	res = register_chrdev(MAJOR_DEVICE_NUN, DEVICE_NAME, &fops);

	if (res < 0) {
		printk(KERN_ALERT "Registering char device failed with %d\n", res);
		return res;
	}

	printk(KERN_INFO "I was assigned major number %d. To talk to\n", MAJOR_DEVICE_NUN);
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, MAJOR_DEVICE_NUN);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");

#if 0
	if (gpionum != 0)
	{
		printk(KERN_INFO "Gpio allocation ok for %d\n", gpionum);
		gpio_export(gpionum, 0);
		gpio_direction_output(gpionum, 1);

		m_moduleAlive = 1;
	}
#endif

	return SUCCESS;

}

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *filp) {
	static int counter = 0;

	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	sprintf(msg, "I already told you %d times Hello world!\n", counter++);
	msg_Ptr = msg;
	/*
	 * TODO: comment out the line below to have some fun!
	 */
	try_module_get(THIS_MODULE);

	return SUCCESS;
}

static long device_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{

	switch (cmd)
	{
		case START_UPLOAD:
			printk(KERN_INFO "\nStart upload in kernel..\n");
		break;
		case ENABLE_DISABLE_LOADER:
			printk(KERN_INFO "\nenable disable in kernel %d..\n" , *(int *)arg);
		break;
		default:
		return -ENOTTY;
	}

	return SUCCESS;
}




/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *filp)
{
	Device_Open--;

	if (pKernelBuffer != NULL)
	{
		kfree(pKernelBuffer);
		pKernelBuffer = NULL;
	}

	/*
	 * Decrement the usage count, or else once you opened the file, you'll never
	 * get rid of the module.
	 *
	 * TODO: comment out the line below to have some fun!
	 */
	module_put(THIS_MODULE);

	return SUCCESS;
}

/*
 * Called when a process, which already opened the dev file, attempts to read
 * from it.
 */
static ssize_t device_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	/*
	 * Number of bytes actually written to the buffer
	 */
	int bytes_read = 0;

	/*
	 * If we're at the end of the message, return 0 signifying end of file.
	 */
	if (*msg_Ptr == 0)
		return 0;

	/*
	 * Actually put the data into the buffer
	 */
	while (length && *msg_Ptr) {
		/*
		 * The buffer is in the user data segment, not the kernel segment so "*"
		 * assignment won't work. We have to use put_user which copies data from the
		 * kernel data segment to the user data segment.
		 */
		put_user(*(msg_Ptr++), buffer++);
		length--;
		bytes_read++;
	}

	/*
	 * Most read functions return the number of bytes put into the buffer
	 */
	return bytes_read;
}

/*
 * Called when a process writes to dev file: echo "hi" > /dev/hello
 */
static ssize_t device_write(struct file *filp, const char *buf, size_t len,	loff_t *off)
{

	int retval = -EFAULT;

	printk(KERN_ALERT "Writing the image to the kernel.\n");

	if (pKernelBuffer != NULL)
	{
		kfree(pKernelBuffer);
		pKernelBuffer = NULL;
	}

	if (pKernelBuffer == NULL && len > 0)
		pKernelBuffer = kmalloc(len, GFP_KERNEL);

	if (len > 0)
	{
		if (copy_from_user(pKernelBuffer, buf, len) != 0){
			retval = -EFAULT;
		}
		else
		{
			retval = len;
		}
	}

	return retval;
}

static void __exit piccolobl_cleanup(void)
{

	if (pKernelBuffer != NULL)
	{
		kfree(pKernelBuffer);
		pKernelBuffer = NULL;
	}


	unregister_chrdev(MAJOR_DEVICE_NUN, DEVICE_NAME);

	printk(KERN_INFO "Piccolo bootloader cleanup!\n");
	m_moduleAlive = 0;

}
module_init( piccolobl_init);
module_exit( piccolobl_cleanup);

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>   

MODULE_LICENSE("GPL");

/* 	Function prototypes  	*/

int init_module(void);
void cleanup_module(void);
static int device_open(struct inode*, struct file*);
static int device_release(struct inode*, struct file*);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);

/*	Defines		*/

#define DEVICE_NAME "keylogger_dev"
#define BUFFER_LEN 100

/*	Global variables	*/

static int major_num;
static int device_open_count = 0;
static char msg[BUFFER_LEN];
static char *msg_ptr;

static struct file_operations file_ops = {
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release
};

/*	Functions	*/

static int __init keylogger_init(void)
{
	major_num = register_chrdev(0, DEVICE_NAME, &file_ops); 
	
	if (major_num < 0) 
	{
		printk(KERN_ALERT "register_chrdev failed: %d\n", major_num);
		return major_num;
	}
	else 
	{
		printk(KERN_INFO "keylogger_device device major number %d\n", major_num);
		return 0;
	}
}

static void __exit keylogger_exit(void)
{
	unregister_chrdev(major_num, DEVICE_NAME);
	printk(KERN_INFO "Keylogger exited\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);

static ssize_t device_read(struct file* flip, 
		char* msg_buffer, 
		size_t len, 
		loff_t* offset) 
{
	int bytes_read = 0;
	
	if (*msg_ptr == 0) 
	{
		msg_ptr = msg_buffer;
	}	
	while (len && *msg_ptr) 
	{
		put_user(*(msg_ptr++), msg_buffer++);
		len--;
		bytes_read++;
	}

	return bytes_read;
}

static ssize_t device_write(struct file* flip, 
		const char* msg_buffer, 
		size_t len, 
		loff_t* offset) 
{
	printk(KERN_ALERT "Can't write to keyboard device.\n");
	return -EINVAL;
}

static int device_open(struct inode* inode, struct file* file)
{
	if (device_open_count)
	{
		return -EBUSY;
	}
	device_open_count++;
	try_module_get(THIS_MODULE);
	return 0;
}

static int device_release(struct inode* inode, struct file* file)
{
	device_open_count--;
	module_put(THIS_MODULE);
	return 0;
}

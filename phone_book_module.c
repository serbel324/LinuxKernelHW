#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

#define MSG_BUFFER_LEN 16

static int device_open(struct inode*, struct file*);
static int device_release(struct inode*, struct file*);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);

static int major_num;
static int device_open_count = 0;
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;

struct device_data {
	struct cdev cdev;
	struct idr phone_book;
};

static struct file_operations file_ops = {
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

static ssize_t device_read(struct file* flip, char* buffer, size_t len, loff_t* offset) {
	struct device_data *dev_data;
	dev_data = (struct device_data*) file->private_data;
	int bytes_read = 0;
	
	if (*msg_ptr == 0) {
		msg_ptr = msg_buffer;
	}

	while (len && *msg_ptr) {
		put_user(*(msg_ptr++), buffer++);
		len--;
		bytes_read++;
	}
	return bytes_read;
}

static ssize_t device_write(struct file* flip, const char* buffer, size_t len, loff_t* offset) {
	printk(KERN_ALERT "Invalid attempt to write to device.\n");
	return -EINVAL;
}

static int device_open(struct inode* inode, struct file* file) {
	if (device_open_count) {
		return -EBUSY;
	}

	device_open_count++;
	try_module_get(THIS_MODULE);

	struct device_data *dev_data;
	dev_data = container_of(inode->i_cdev, struct device_data, cdev);
	file->private_data = dev_data;

	return 0;
}

static int device_release(struct inode* inode, struct file* file) {
	device_open_count--;
	module_put(THIS_MODULE);
	return 0;
}

static int __init phone_book_module_init(void) {
	major_num = register_chrdev(0, "phone_book_device", &file_ops);

	if (major_num < 0) {
		printk(KERN_ALERT "Couln not register device: %d \n", major_num);
		return major_num;
	}

	printk(KERN_INFO "phone_book_device device major number %d\n", major_num);
	return 0;
}
static void __exit phone_book_module_exit(void) {
	unregister_chrdev(major_num, "phone_book_device");
}

module_init(phone_book_module_init);
module_exit(phone_book_module_exit);

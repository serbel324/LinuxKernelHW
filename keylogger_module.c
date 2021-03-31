#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/unistd.h>

#include <linux/fs.h>
#include <asm/io.h>   

MODULE_LICENSE("GPL");

/* 	Function prototypes  	*/

static int __init keylogger_init(void);
static void __exit keylogger_exit(void);
static irqreturn_t irq_handler(int, void*);
void timer_callback(struct timer_list* timer);

/*	Defines		*/

#define INTERRUPT_NAME "keylogger"
#define LOG_INTERVAL 60000

/*	Global variables	*/

static volatile unsigned int irq_counter;
static struct timer_list log_timer;

/*	Functions	*/

static int __init keylogger_init(void)
{
	printk(KERN_INFO "Keylogger loaded.\n");
	free_irq(1, NULL);	
	irq_counter = 0;
	
	timer_setup(&log_timer, timer_callback, 0);
	mod_timer(&log_timer, jiffies + msecs_to_jiffies(LOG_INTERVAL));

	return request_irq(1, 
			irq_handler, 
			IRQF_SHARED, 
			INTERRUPT_NAME,
		       	(void*)(irq_handler));
}

static void __exit keylogger_exit(void)
{
	free_irq(1, NULL);
	del_timer(&log_timer);

	printk(KERN_INFO "Keylogger unloaded\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);

static irqreturn_t irq_handler(int irq, void* dev)
{
	static unsigned char scancode;
	
	scancode = inb(0x60);
	
	if (scancode & 0x80)
		irq_counter++;
	
	return IRQ_HANDLED;
}

void timer_callback(struct timer_list* timer)
{
	printk(KERN_INFO "keys pressed %d times", irq_counter);
	irq_counter = 0;
	mod_timer(&log_timer, jiffies + msecs_to_jiffies(LOG_INTERVAL));
}

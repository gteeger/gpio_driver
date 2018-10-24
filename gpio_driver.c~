/*
TODO:

1) Write a Keypad Driver:

- Implement blocking read call
- Implement singal handler
- Register IRQ pin of your desired GPIO

Description: Should have poll(), select() , and signal generator.

Reference : https://static.lwn.net/images/pdf/LDD3/ch06.pdf

2) Write Application:

- Open FD from above driver in blocking mode - Use select() system call to wait,
on it. Condition: Your application must register USER define signal handler, and
SIGTERM and SIGKILL, and User define signal of your choice you selected in the
keypad driver. Reference : https://moodle2.units.it/pluginfile.php/110718/mod_re
source/content/2/Linux%20System%20Programming%2C%202nd%20Edition.pdf chapter 10

*/

#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/spinlock.h>



#define MAJOR_NUM 420
#define DEVICE_NAME "GPIO_DRIVER"
#define BUTTON1_GPIO 115
#define DEBOUNCE_TIME 200
#define TRUE (1)
#define FALSE (0)
#define MAX_BUFFER_SIZE (512)

#define HI (1)
#define LO (0)

static int irqNumber;
static int button_presses;
static int read_index, write_index;
static int is_data;
static int irq_data;

/* The next line is equivalent to: static wait_queue_head_t gpio_waitqueue;*/
static DECLARE_WAIT_QUEUE_HEAD(gpio_waitqueue);

static DEFINE_SPINLOCK(gpio_lock);


static char buffer[MAX_BUFFER_SIZE] = { 0 };


static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static irqreturn_t irq_handler(unsigned int irq, void *dev_id,
			       struct pt_regs *regs);
static unsigned int gpio_poll(struct file *file, poll_table * wait);

static struct file_operations fops = {
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
    .poll = gpio_poll,
};



static int __init mod_init(void)
{

    int result;
    unsigned long IRQflags;
    IRQflags = IRQF_TRIGGER_RISING;
    button_presses = 0;
    read_index = 0;
    write_index = 0;
    printk(KERN_INFO "gpio driver initialized\n");

    /* Register the character device */
    if (register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops)) {
	printk(KERN_ALERT "FAILED TO REGISTER MAJOR NUMBER\n");
	return -1;
    }
/* Debounce time is in milliseconts */
    if (gpio_request(BUTTON1_GPIO, "button1_gpio") ||
	gpio_set_debounce(BUTTON1_GPIO, DEBOUNCE_TIME) ||
	gpio_direction_input(BUTTON1_GPIO)) {

	printk(KERN_ALERT "FAILED TO EXPORT gpio pin");
	return -1;
    }

    printk(KERN_INFO "Registered correctly with major number correctly\n");
    printk(KERN_INFO "The button state is currently: %d\n",
	   gpio_get_value(BUTTON1_GPIO));

    /* Interrupt used to indicate the presense of data */
    irqNumber = gpio_to_irq(BUTTON1_GPIO);
    printk(KERN_INFO "The button is mapped to IRQ: %d\n", irqNumber);
    result = request_irq(irqNumber,
			 (irq_handler_t) irq_handler,
			 IRQflags, "button_handler", NULL);
    return result;


}


static void __exit mod_exit(void)
{

    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);

    printk(KERN_INFO "gpio exited\n");
}


/** @brief The device open function that is called each time the device is opened
 *  This will only increment the numberOpens counter in this case.
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_open(struct inode *inodep, struct file *filep)
{

    return 0;
}

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char *in_buffer, size_t len,
			loff_t * offset)
{

    ssize_t retval;
    DECLARE_WAITQUEUE(waitq, current);
    add_wait_queue(&gpio_waitqueue, &waitq);

    do {
	set_current_state(TASK_INTERRUPTIBLE);

	if (is_data == TRUE)
	    break;
	if (filep->f_flags & O_NONBLOCK) {
	    retval = -EAGAIN;
	    goto out;
	}
	schedule();
    } while (1);

    retval = copy_to_user(in_buffer, buffer, write_index);
    if (!retval)
	retval = write_index;

    /* We have read everything, there is no new data */
    is_data = FALSE;
    write_index = 0;

  out:
    set_current_state(TASK_RUNNING);
    remove_wait_queue(&gpio_waitqueue, &waitq);

    return retval;

}


/** 
 * @brief The device release function that is called whenever the device is closed/released by
 *  the userspace program
 *  @param inodep A pointer to an inode object (defined in linux/fs.h)
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 */
static int dev_release(struct inode *inodep, struct file *filep)
{
    free_irq(irqNumber, NULL);
    gpio_unexport(BUTTON1_GPIO);
    gpio_free(BUTTON1_GPIO);
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static irqreturn_t irq_handler(unsigned int irq, void *dev_id,
			       struct pt_regs *regs)
{

    spin_lock(&gpio_lock);
    irq_data = HI;
    buffer[write_index] = irq_data;
    write_index++;
    spin_unlock(&gpio_lock);
    is_data = TRUE;
    button_presses++;

    /* Wake up processes in interruptible sleep */
    wake_up_interruptible(&gpio_waitqueue);
    return IRQ_HANDLED;
}

static unsigned int gpio_poll(struct file *file, poll_table * wait)
{

    poll_wait(file, &gpio_waitqueue, wait);

    return (is_data != FALSE) ? (POLLIN | POLLRDNORM) : FALSE;
}


module_init(mod_init);
module_exit(mod_exit);


#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/delay.h>

int gpio_in=49; /*GPIO1_7*/
int gpio_out=10;
int unhandled_irq = 0;

static int gpio_irq_num = 0;

module_param(gpio_in,int,0);
module_param(gpio_out,int,0);
module_param(unhandled_irq,int,0644);

irqreturn_t gpio_irq_handler(int irq, void * ident)
{
	static int value = 1;
  	struct irq_desc *desc = irq_to_desc(gpio_irq_num);
	unhandled_irq = desc->irqs_unhandled;
  	
	pr_info("current unhandled irqs count %d", unhandled_irq);
	pr_info("MIR: Total irq cnt - %d\n", desc->irq_count);

	gpio_set_value(gpio_out, value);
  	//printk ("ISR\n");
  	value = 1 - value;

  	//mdelay(3);//1900 + 150); //arg - usec delay
  	//return IRQ_HANDLED;
  	//return IRQ_NONE;
	
	return IRQ_WAKE_THREAD;
}

/*
** This function is the threaded irq handler
*/
static irqreturn_t gpio_irq_thread_fn(int irq, void *dev_id) 
{
	static unsigned int threaded_irq_cnt = 0;
	threaded_irq_cnt++;
	pr_info("MIR: Threaded irq invoked : %d times\n", threaded_irq_cnt);
  
	while(1){
		mdelay(1000);//arg - usec delay
	}
  	
	return IRQ_HANDLED;
}

int example_init (void)
{
	int err;
  	struct irq_desc *my_irq_desc;

  	gpio_irq_num = gpio_to_irq(gpio_in);

   	printk("interrupt request number:%d",gpio_irq_num);

  	if ((err = gpio_request(gpio_out, THIS_MODULE->name)) != 0)
    		return err;
  	if ((err = gpio_direction_output(gpio_out, 0)) != 0) {
    		gpio_free(gpio_out);
    		return err;
  	}

  	if ((err = gpio_request(gpio_in, THIS_MODULE->name)) != 0) {
    		gpio_free(gpio_out);
   		return err;
  	}

  	if ((err = gpio_direction_input(gpio_in)) != 0) {
    		gpio_free(gpio_in);
    		gpio_free(gpio_out);
    		return err;
 	 }

  	if ((err = request_threaded_irq(gpio_irq_num, gpio_irq_handler, 
					gpio_irq_thread_fn, IRQF_TRIGGER_RISING, "gpio_device", NULL)) != 0) {
    		gpio_free(gpio_in);
    		gpio_free(gpio_out);
    		return err;
  	}
  	//irq_set_irq_type(gpio_irq, IRQF_TRIGGER_RISING);

  	my_irq_desc = irq_to_desc(gpio_irq_num);
  	unhandled_irq = my_irq_desc->irq_count;
  
  	return 0;
}

void example_exit (void)
{
	free_irq(gpio_irq_num, THIS_MODULE->name);
  	gpio_free(gpio_in);
  	gpio_free(gpio_out);
}

EXPORT_SYMBOL(unhandled_irq);
module_init(example_init);
module_exit(example_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mir.Faisal2@harman.com");

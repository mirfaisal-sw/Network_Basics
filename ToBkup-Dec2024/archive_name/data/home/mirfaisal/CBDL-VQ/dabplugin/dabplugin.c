#define pr_fmt(fmt) "DAB PLUGIN : "fmt
#include <linux/kernel.h>
#include <linux/spi/spi.h>
#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/kfifo.h>
#include <linux/completion.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <generated/compile.h>
#define MAX_NAME_SIZE 32
#define DAB_NUM_BOOT_MSG   2
#define DAB_MSG_HEADER_LEN 4
#define DAB_MAX_PKT_SIZE          5472

struct dabdrv {
	int gpio_irq_num, gpio_num;
	struct spi_device *spi;
	char ifacename[MAX_NAME_SIZE];
	struct completion rx_data_avail;
	/*TODO : allocate fifo dynamically so that fifo size can be changed */
	DECLARE_KFIFO(rx_fifo, unsigned char, 8192);
	spinlock_t txrx_lock;
	int exiting:1;
	int initcomplete:1;

	struct mutex rlock, wlock;

	unsigned char *rxbuf, *txbuf;
	int rxbuf_size, txbuf_size;

	int major_num;
	struct device *dev;
	struct class *class;
};
static struct dabdrv dab;

static unsigned int fifo_depth = 128, fifo_width = 5472, q_almost_full = 120;
#if defined (CONFIG_IUKRAM_MID)
static unsigned int gpio_num = 68;
#elif defined (CONFIG_IUKRAM_HIGH)
static unsigned int gpio_num = 230;
#elif defined (CONFIG_NTG6_CSB_M127)
static unsigned int gpio_num = 90;
#else
#warning "DABPLUGIN BUILD FOR UNKNOWN ARCH!!!, PASS GPIO INFO WHILE LOADING MODULE"
static unsigned int gpio_num;
#endif

module_param(fifo_width, uint , 0);
MODULE_PARM_DESC(fifo_width, "FIFO Width");
module_param(fifo_depth, uint, 0);
MODULE_PARM_DESC(fifo_depth, "FIFO Depth");
module_param(gpio_num, uint, 0);
MODULE_PARM_DESC(gpio_num, "GPIO Num");
module_param(q_almost_full, uint, 0);
MODULE_PARM_DESC(q_almost_full, "Queue Almost Full");

static char *char_iface_name = "spi0";
module_param(char_iface_name, charp, 0660);
MODULE_PARM_DESC(char_iface_name, "Char Interface Name");

#ifdef BUILDID_STR
MODULE_INFO(buildid, BUILDID_STR" "UTS_VERSION);
#endif

void omap2_mcspi_force_cs(struct spi_device *spi, int cs_active);
static void spi_msg_complete_cb(void *context)
{
	pr_debug("\n");
	return;
}

static int spi_xchange(struct spi_device *d, unsigned char *txbuf,
			unsigned char *rxbuf, int len, int cs_change)
{
	struct spi_message m;
	struct spi_transfer t;
	int ret;

	memset(&t, 0, sizeof(t));
	t.tx_buf = txbuf;
	t.rx_buf = rxbuf;
	t.len = len;
	t.cs_change = cs_change;
	spi_message_init(&m);
	m.complete = spi_msg_complete_cb;
	spi_message_add_tail(&t, &m);
	ret = spi_async(d, &m);
	if (ret)
		pr_err("Transfer failed for %s TXBUF %p RXBUF %p LEN %x"
			" CS %x\n", dev_name(&d->dev), txbuf, rxbuf,
				len, cs_change);
	else
		pr_debug("Transfer success for %s TXBUF %p RXBUF %p LEN %x"
				" CS %x\n", dev_name(&d->dev), txbuf, rxbuf,
				len, cs_change);
	return ret;
}


static int dab_open(struct inode *i, struct file *f)
{
	return 0;
}

static ssize_t dab_read(struct file *f, char __user *buf, size_t count,
						loff_t *offset)
{
	static int callseq = 0;
	int res, copied = 0;
	callseq++;
	if (dab.exiting)
		return -EBUSY;
	mutex_lock(&dab.rlock);
	while (kfifo_is_empty(&dab.rx_fifo)) {
		pr_debug("[%d]:No Data available in Kfifo, going to wait\n",
						callseq);
		res = wait_for_completion_interruptible(&dab.rx_data_avail);
		if (res) {
			pr_err("Read was interrupted [%d] !!!\n", res);
			copied = res;
			goto ret;
		}
	}
	res = kfifo_to_user(&dab.rx_fifo, buf, count, &copied);
	if (res) {
		pr_err("[%d]ERROR : Reading %d data from kfifo\n", callseq,
								count);
		copied = res;
		goto ret;
	}
ret:
	mutex_unlock(&dab.rlock);
	if (!copied) {
		pr_err("[%d]ERROR : Read Returning 0!!!.\n", callseq);
		pr_err("[%d]->buf[%p] count[%d], offset[%p], res[%d],"
				" copied[%d]\n", callseq, buf, count, offset,
				res, copied);
		pr_err("[%d]->kfifo is %s\n", callseq,
			kfifo_is_empty(&dab.rx_fifo) ? "empty" : "not empty");
		pr_err("[%d]->kfifo_len = %d\n", callseq,
					kfifo_len(&dab.rx_fifo));
		pr_err("[%d]->kfifo.in = %d kfifo.out = %d kfifo.mask = %d "
			"kfifo.esize = %d kfifo.data = %p\n", callseq,
			dab.rx_fifo.kfifo.in, dab.rx_fifo.kfifo.out,
			dab.rx_fifo.kfifo.mask, dab.rx_fifo.kfifo.esize,
			dab.rx_fifo.kfifo.data);
	}
	return copied;
}

static int _send_spi_data_n_put_in_kfifo(unsigned char *txbuf, int txlen,
						int userwrite);

static ssize_t dab_write(struct file *f, const char __user *buf,
		size_t count, loff_t *offset)
{
	unsigned long flag = 0;
	int res;

	if (dab.exiting)
		return -EBUSY;
	mutex_lock(&dab.wlock);
	spin_lock_irqsave(&dab.txrx_lock, flag);
	if (count >= dab.txbuf_size) {
		pr_err("Size[%d] too big to write\n", count);
		count = -1;
		goto unlock_n_ret;
	}
	memset(dab.txbuf, 0, count + 1);
	res = copy_from_user(dab.txbuf, buf, count);
	if (res) {
		pr_err("Error in accessing user buffer to write\n");
		goto unlock_n_ret;
	}
	print_hex_dump_debug("DAB PLUGIN : TX APP : ", DUMP_PREFIX_OFFSET,
				16, 1, dab.txbuf, count, 0);

	res = _send_spi_data_n_put_in_kfifo(dab.txbuf, count, 1);
	if (res) {
		pr_err("Error communication with DAB Chip\n");
		goto unlock_n_ret;
	}

unlock_n_ret:
	spin_unlock_irqrestore(&dab.txrx_lock, flag);
	mutex_unlock(&dab.wlock);
	return count;
}

static int dab_release(struct inode *i, struct file *f)
{
	return 0;
}

static struct file_operations dab_fops = {
	.open = dab_open,
	.read = dab_read,
	.write = dab_write,
	.release = dab_release,
};

static unsigned char dabbootmsg[DAB_NUM_BOOT_MSG+1][DAB_MSG_HEADER_LEN] =
{
	{0xAB,0xBC,0xCD,0xDE},
	{0xE2,0x00,0x10,0x00},
	{0x00,0x00,0x00,0x00}
};

static unsigned int is_valid_dabheader(unsigned char *dh, int len)
{
	unsigned int v;
	v = dh && dh[0] == 0xAD && (len >= DAB_MSG_HEADER_LEN) &&
		( (((dh[2] << 8) | dh[3]) + 2) <= DAB_MAX_PKT_SIZE );
	return v;
}

static int _send_spi_data_n_put_in_kfifo(unsigned char *txbuf, int txlen,
						int userwrite)
{
	int res = -1, pktsize = 0, pktlen = 0;
	unsigned char *pkt = NULL, *dabpkt = NULL;
	
	memset(dab.rxbuf, 0, dab.rxbuf_size);
	omap2_mcspi_force_cs(dab.spi, 1);
	res = spi_xchange(dab.spi, txbuf, dab.rxbuf, txlen, 0);
	if (res) {
		pr_err("xchange failed[%d]\n", res);
		goto ret;
	}
	print_hex_dump_debug("DAB PLUGIN : RX : ", DUMP_PREFIX_OFFSET,
				16, 1, dab.rxbuf, txlen, 0);
	res = is_valid_dabheader(dab.rxbuf, DAB_MSG_HEADER_LEN);
	if (!res) {
		if (!userwrite)
			pr_err("Invalid Packet from DAB\n");
		res = 0;
		goto ret;
	}
	pktlen = (dab.rxbuf[2] << 8) | dab.rxbuf[3];
	pktlen += 2; //for checksum
	pr_debug("RXD Len = %d DAB PKT LEN = %d\n", txlen, pktlen);
	if (txlen >= (pktlen + DAB_MSG_HEADER_LEN)) {
		pr_debug("Full Pkt Rxd\n");
		pkt = dab.rxbuf;
		pktsize = pktlen + DAB_MSG_HEADER_LEN;
	} else {
		pr_debug("Partial Pkt Rxd. Pending %d bytes\n",
						pktlen - DAB_MSG_HEADER_LEN);
		dabpkt = kzalloc(pktlen + DAB_MSG_HEADER_LEN, GFP_ATOMIC);
		if (IS_ERR(dabpkt)) {
			pr_err("Mem Alloc Failed for Partial Pkt\n");
			res = PTR_ERR(dabpkt);
			goto ret;
		}
		memcpy(dabpkt, dab.rxbuf, txlen);
		res = spi_xchange(dab.spi, dabpkt + txlen, dab.rxbuf,
			pktlen - (txlen - DAB_MSG_HEADER_LEN), 0);
		if (res) {
			pr_err("Error reading pending data\n"); 
			goto free_buf_n_ret;
		}
		print_hex_dump_debug("DAB PLUGIN : REM RX : ",
					DUMP_PREFIX_OFFSET, 16, 1, dab.rxbuf,
					pktlen - (txlen - DAB_MSG_HEADER_LEN),
					0);
		memcpy(dabpkt + txlen, dab.rxbuf, pktlen);
		pktsize = DAB_MSG_HEADER_LEN + pktlen;
		pkt = dabpkt;
	}
	print_hex_dump_debug("DAB PLUGIN : RX PKT : ",
					DUMP_PREFIX_OFFSET, 16, 1, pkt,
					pktsize, 0);
	if (kfifo_is_full(&dab.rx_fifo)) {
		pr_err("RX : KFIFO IS FULL DROPING DATA!!!!!!!!!\n");
		goto free_buf_n_ret;
	}
	res = kfifo_avail(&dab.rx_fifo);
	if (res < pktsize) {
		pr_err("RX : KFIFO : NO REQ FREE SPACE[A:%d - R:%d]"
				" DROPING DATA!!!!!!!!!\n", res, pktsize);
		goto free_buf_n_ret;
	}
	res = kfifo_in(&dab.rx_fifo, pkt, pktsize);
	if (res != pktsize) {
		pr_err("RX : KFIFO : ERROR [pktsize = %d, data put into fifo "
			"= %d\n", pktsize, res);
		goto free_buf_n_ret;
	}
	res = 0;
	complete(&dab.rx_data_avail);
free_buf_n_ret:
	kfree(dabpkt);
ret:
	omap2_mcspi_force_cs(dab.spi, 0);
	return res;
}

static irqreturn_t dab_irq_handler(int i, void *a)
{
	static int irqs, lock;
	int sendlen = DAB_MSG_HEADER_LEN, res = 0;
	unsigned char *txbuf;

	lock = spin_trylock(&dab.txrx_lock);
	if (!lock) {
		pr_info("Seems TX is holding lock, ret immediatly\n");
		return IRQ_HANDLED;
	}
	pr_debug("IRQ == %d\n", irqs);
	if (irqs < 2)
		txbuf = dabbootmsg[irqs];
	else
		txbuf = dabbootmsg[2];
	irqs++;
	res = _send_spi_data_n_put_in_kfifo(txbuf, sendlen, 0);
	if (res)
		pr_err("Problem in communication with DAB chip\n");

	spin_unlock(&dab.txrx_lock);
	return IRQ_HANDLED;
}

static int dab_probe(struct spi_device *spi)
{
	int result = 0;

	dab.spi = spi;
	dab.gpio_num = gpio_num;
	strlcpy(dab.ifacename, char_iface_name, 20);
	INIT_KFIFO(dab.rx_fifo);
	init_completion(&dab.rx_data_avail);
	spin_lock_init(&dab.txrx_lock);

	dab.major_num = register_chrdev(0, dab.ifacename, &dab_fops);
	if (dab.major_num < 0) {
		pr_err("Device couldn't be registered!");
		result = dab.major_num;
		goto err_reg_chrdev;
	}
	dab.class = class_create(THIS_MODULE, dab.ifacename);
	if (IS_ERR(dab.class)) {
		pr_err("Error %ld creating %s device class \n",
				PTR_ERR(dab.class), dab.ifacename);
		result = PTR_ERR(dab.class);
		goto err_class_create;
	}
	dab.dev = device_create(dab.class, NULL, MKDEV(dab.major_num, 0),
					NULL, dab.ifacename);
	if (IS_ERR(dab.dev)) {
		pr_err("error creating device\n");
		result = PTR_ERR(dab.dev);
		goto err_device_create;
	}
	dab.txbuf = kmalloc(6000, GFP_KERNEL);
	if (IS_ERR(dab.txbuf)) {
		result = PTR_ERR(dab.txbuf);
		goto err_txbuf_alloc_failed;
	}
	dab.txbuf_size = 6000;

	dab.rxbuf = kmalloc(6000, GFP_KERNEL);
	if (IS_ERR(dab.rxbuf)) {
		result = PTR_ERR(dab.rxbuf);
		goto err_rxbuf_alloc_failed;
	}
	dab.rxbuf_size = 6000;
	if (!gpio_is_valid(dab.gpio_num)) {
		pr_err("Invalid gpio number %d for dab irq pin\n",
						dab.gpio_num);
		goto err_gpio_invalid;
	}
	result = gpio_request(dab.gpio_num, "dab_irq_pin");
	if (result) {
		pr_err("Claiming gpio pin %d failed!\n",
						dab.gpio_num);
		goto err_gpio_req_failed;
	}
	dab.gpio_irq_num = gpio_to_irq(dab.gpio_num);
	result = gpio_direction_input(dab.gpio_num);
	if (result) {
		pr_err("Unable to set gpio %d as input for dab interrupt\n",
					dab.gpio_num);
		goto err_gpio_set_direction;
	}
	mutex_init(&dab.rlock);
	mutex_init(&dab.wlock);
	result = request_irq(dab.gpio_irq_num, dab_irq_handler,
				IRQF_TRIGGER_FALLING,
				"dabplugin", (void*)&spi->dev);
	if (result) {
		pr_err("requesting irq failed for irqnum = %d"
			" (gpio num = %d)\n", dab.gpio_irq_num,
			dab.gpio_num);
		goto err_req_irq_failed;
	}
	pr_debug("Requesting gpio[%d] irq[%d] success\n", 
			dab.gpio_num, dab.gpio_irq_num);
	if (gpio_export(dab.gpio_num, false)) {
		pr_err("Non Fatal : GPIO Export Failed\n");
	}
	dab.initcomplete = 1;
	return 0;

err_req_irq_failed:
	mutex_destroy(&dab.rlock);
	mutex_destroy(&dab.wlock);
err_gpio_set_direction:
	gpio_free(dab.gpio_num);
err_gpio_req_failed:
err_gpio_invalid:
	kfree(dab.rxbuf);
	dab.rxbuf_size = 0;
err_rxbuf_alloc_failed:
	kfree(dab.txbuf);
	dab.txbuf_size = 0;
err_txbuf_alloc_failed:
	device_destroy(dab.class, MKDEV(dab.major_num, 0));
err_device_create:
	class_destroy(dab.class);
err_class_create:
	unregister_chrdev(dab.major_num, dab.ifacename);
err_reg_chrdev:
	return result;
}

static int dab_remove(struct spi_device *dev)
{

	if (!dab.initcomplete)
		return 0;
 	//free the irq
	free_irq(dab.gpio_irq_num, &dab.spi->dev);
	gpio_free(dab.gpio_num);
	dab.exiting = 1;
	complete_all(&dab.rx_data_avail);
	mutex_lock(&dab.rlock);
	device_destroy(dab.class, MKDEV(dab.major_num, 0));
	class_destroy(dab.class);
	unregister_chrdev(dab.major_num, dab.ifacename);
	kfree(dab.rxbuf);
	kfree(dab.txbuf);

	//make sure that all the blocked userspace calls has been released
	return 0;
}

#ifdef CONFIG_OF
static struct of_device_id dabplugin_of_match[] = {
	{
		.compatible = "dabplugin",
	},
	{}
};
MODULE_DEVICE_TABLE(of, dabplugin_of_match);
#endif

static const struct spi_device_id dab_id[] = {
	{ "dabplugin", 0 },
	{ }
};
MODULE_DEVICE_TABLE(spi, dab_id);

static struct spi_driver dab_driver = {
	.driver = {
		.name = "dabplugin",
		.owner = THIS_MODULE,
#ifdef CONFIG_OF
		.of_match_table = of_match_ptr(dabplugin_of_match),
#endif
	},
	.probe = dab_probe,
	.remove = dab_remove,
	.id_table = dab_id,
};

static int plugin_init(void)
{
	return spi_register_driver(&dab_driver);
}

static void plugin_destroy(void)
{
	spi_unregister_driver(&dab_driver);
}

module_init(plugin_init);
module_exit(plugin_destroy);
MODULE_LICENSE("GPL");

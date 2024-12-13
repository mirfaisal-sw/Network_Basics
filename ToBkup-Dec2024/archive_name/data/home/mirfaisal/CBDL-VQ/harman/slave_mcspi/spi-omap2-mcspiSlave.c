/*
 *Description  : To configure MCSPI in slave mode, and to provide 
 *               character driver interface for user space application
 *
 *Major -
 *Design Goals : - Driver should be usable across multiple chipsets (j5, j6..)
 *               - Driver should support EDMA and SDMA
 *               - Give precedence to Lock free programming                
 *
 *Author       : Padmanabha.S <padmanabha.srinivasaiah@harman.com>
 *
 *Copyright (c) 2016 Harman International Industries [connected car]
 *
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/types.h>         /* for uint16_t... */
#include <linux/fs.h>            /* register_chrdev_region().... */
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/printk.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pm_runtime.h>
#include <linux/delay.h>
#include <linux/spinlock.h>
#include <linux/dmaengine.h>

#include <linux/slab.h>            /* for kmalloc() */
#include <linux/io.h>              /* for readl(), writel() */
#include <linux/dma-mapping.h>
#include <linux/omap-dmaengine.h>  /* for omap_dma_filter_fn */
#include <linux/stddef.h>          /* true/false :) */
#include <linux/semaphore.h>       /* for sema_init() */
#include <linux/version.h>         /* for LINUX_VERSION_CODE, KERNEL_VERSION */
#include <linux/atomic.h>          /* for atomic_read(), atomic_set() */

#include <linux/platform_data/spi-omap2-mcspi.h>

#include <asm-generic/uaccess.h>   /* for copy_to_user() */

#include "spi-omap2-mcspiSlave.h"

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,37)
#define init_MUTEX(sem)         sema_init(sem, 1)
#endif


#ifdef DEBUG_DEV
#define dbg_print(args...)        printk(KERN_ALERT args);
#else
#define dbg_print(args...)
#endif

#define DEV_NAME       "spiATmegaVics"
#define CLASS_NAME     "spi_slave4vics"

#define DEVICES        2 /* one for read, one for write/sw update??? */
#define DEVICES        2
#define DARC_DATA_DEV  0 

static uint16_t frame_sz  = FRAME_SZ;
static uint16_t period_sz = PERIOD_SZ;
static uint16_t periods   = PERIODS;
static uint16_t buf_sz    = BUFFER_SZ;
static uint8_t major      = 0;

/* for sysfs interface */
extern struct kobj_type slave_kobj_type;
extern uint32_t get_period_sz(void);

module_param(frame_sz, ushort, 0444);    /*world readable */
module_param(period_sz, ushort, 0444);   /*doesnt recognize uint16_t... */
module_param(periods, ushort, 0444);
module_param(buf_sz, ushort, 0444);
module_param(major, byte, 0444);


typedef struct spiATmegaVics_dev {
    circ_buff_t *rd_buff;
    struct device *dev;    
    omap2_slave_mcspi_t     *slave;

    wait_queue_head_t wait_4vnt;

    struct semaphore dev_sem;
    
    struct cdev cdev;
    dev_t dev_nr;

    struct class *class;
    struct kobject kobj;

    atomic_t    state; 
} spiATmegaVics_dev_t;

spiATmegaVics_dev_t vics_dev;
omap2_slave_mcspi_t     *slave;

/*function prototypes... for forward declaration */
static int omap2_slave_mcspi_setup_rxdma ( omap2_slave_mcspi_t *slave);
static void omap2_slave_mcspi_set_dma_req(omap2_slave_mcspi_t *slave,
                                               int is_read, int enable);

/*
 * tools to handle circular buffer...
 */
static inline void flush_buf(circ_buff_t *buf)
{
    pr_debug("@@@@ %s...\n", __func__);

    buf->wr_indx = buf->rd_indx = 0;
    memset(buf->buf_ptr, 0, buf->buf_sz);
}

static inline void init_buf(circ_buff_t *buf, void * buf_ptr,
                             uint16_t buf_sz,
                            uint16_t prd_sz, uint16_t prds)
{
    pr_debug("@@@@ %s...\n", __func__);

    buf->buf_ptr = buf_ptr;
    buf->buf_sz  = buf_sz;
    buf->prd_sz  = prd_sz;
    buf->prds    = prds;
    flush_buf(buf);
}

static inline int is_buf_empty(circ_buff_t *buf)
{
    return (buf->rd_indx == buf->wr_indx)? true:false;
}

static inline int is_buf_full(circ_buff_t *buf)
{
    uint32_t nxt_wrindx = buf->wr_indx + buf->prd_sz;

    if( nxt_wrindx >=  buf->buf_sz)
        nxt_wrindx = 0;

    return (nxt_wrindx == buf->rd_indx)? true: false;
}

static inline void * buf_addr_popup(circ_buff_t *buf)
{
    int ret;
    void *data_ptr = NULL;

    ret = is_buf_empty(buf);
    if(ret == true){
        goto out;
    }

    dbg_print(KERN_ALERT "@@@@ %s: current rd indx %d\n", \
                            __func__, buf->rd_indx/buf->prd_sz);

    data_ptr = buf->buf_ptr + buf->rd_indx;
    buf->rd_indx += buf->prd_sz;

    if(buf->rd_indx >= buf->buf_sz)
        buf->rd_indx = 0;

    dbg_print(KERN_ALERT "@@@@ %s: next rd indx %d\n", \
                            __func__, buf->rd_indx/buf->prd_sz);
out:
    return data_ptr;
}
    

/*
 * tools to access spi ip hw..
 */
static inline uint32_t mcspi_write_reg( \
                           const struct omap2_mcspi_slave *slvmcspi,\
                           uint32_t offset, uint32_t value)
{
    dbg_print(KERN_ALERT "\t\t@@@@ %s: Before wr <%#x> == %#x\n",\
                             __func__, (slvmcspi->phys + offset),\
                                     readl(slvmcspi->base + offset));

    writel(value, slvmcspi->base + offset);


    dbg_print(KERN_ALERT "\t\t@@@@ %s: After wr %#x to <%#x> == %#x\n",\
                           __func__, value,  (slvmcspi->phys + offset),\
                                          readl(slvmcspi->base + offset));
    return 0;
}
                                                                     
static inline uint32_t mcspi_read_reg( \
                           const struct omap2_mcspi_slave *slvmcspi,\
                           uint32_t offset)
{
    uint32_t reg_val = 0;
    
    reg_val = readl(slvmcspi->base + offset);

    dbg_print(KERN_ALERT "@@@@ %s: read %#x from <%#x> \n",\
                          __func__, reg_val, (slvmcspi->phys + offset));
    return reg_val;
}

/*
 * utility to enable/disable fifo
 */
static void omap2_slave_mcspi_fifo_enable(struct omap2_mcspi_slave *slave,
                                                          uint8_t enable)
{
    uint32_t reg_val;

    pr_debug("@@@@ %s :  %d\n", __func__, enable);

    reg_val = mcspi_read_reg(slave, OMAP2_MCSPI_CHCONF0);
    reg_val = enable ? (reg_val | (OMAP2_MCSPI_CHCONF_FFER)) :\
                        (reg_val & ~(OMAP2_MCSPI_CHCONF_FFER));
    mcspi_write_reg(slave, OMAP2_MCSPI_CHCONF0, reg_val);
}

/*
 * utility to enable/disable channel
 */
static void omap2_slave_mcspi_chn_enable(struct omap2_mcspi_slave *slave,
                                                          uint8_t enable)
{
    uint32_t reg_val;

    pr_debug("@@@@ %s :  %d\n", __func__, enable);

    reg_val = enable ? OMAP2_MCSPI_CHCTRL_EN : 0;
    mcspi_write_reg(slave, OMAP2_MCSPI_CHCTRL0, reg_val);
}

 
#ifdef DEBUG_DEV
/*
 * Debug utility... dump MCSPI controller register...
 */
void mcspi_register_trace(const struct omap2_mcspi_slave *slvmcspi)
{
    pr_info("@@@@ %s \n", __func__);
    pr_info("\tREVISION          %#x\n",\
              mcspi_read_reg(slvmcspi, OMAP2_MCSPI_REVISION));
    pr_info("\tSYSCONFIG         %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_SYSCONFIG));
    pr_info("\tSYSSTATUS         %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_SYSSTATUS));
    pr_info("\tIRQSTATUS         %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_IRQSTATUS));
    pr_info("\tIRQENABLE         %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_IRQENABLE));
    pr_info("\tSYSTEST          %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_SYST));
    pr_info("\tMODULECTRL          %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_MODULCTRL));
    pr_info("\tCHCONF0          %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_CHCONF0));
    pr_info("\tCHSTAT0          %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_CHSTAT0));
    pr_info("\tCHCTRL0          %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_CHCTRL0));
    pr_info("\tXFERLEVEL        %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_XFERLEVEL));
    pr_info("\tDAFRX        %#x\n",\
             mcspi_read_reg(slvmcspi, OMAP2_MCSPI_DFRAX));
    return;
}
#endif

/*
 * stream data to a device...
 */
ssize_t slave_intf_write (struct file *file, const char __user * usr_buf,\
                           size_t cnt, loff_t *offset)
{
    /* TODO : stream data into device.. not required as of now */

    return 0;
}


/*
 * retrieve data from device.. (from buffer)...
 */
ssize_t slave_intf_read (struct file *file, char __user * usr_buf,\
                           size_t cnt, loff_t *offset)
{
    spiATmegaVics_dev_t *vics_devp;
    omap2_slave_mcspi_t *slave;
    struct omap2_mcspi_dma *mcspi_dma;
    circ_buff_t * dev_buf;

    int ret = -1;
    int tocpy;
    void *data_ptr;

    pr_debug("@@@@ %s: need to read %d bytes \n", __func__, cnt);

    vics_devp = file->private_data;
    slave = vics_devp->slave;
    dev_buf = &slave->in_buff;
    mcspi_dma = &slave->dma_channel;

    ret = is_buf_empty(&slave->in_buff);
    if(ret == true){
        if(file->f_flags & O_NONBLOCK){
            pr_info("@@@@ %s: no data... user dont want to be blocked!!!\n",
                                                                    __func__);
            ret = -EAGAIN;
            goto OUT;
        }
        else {
            reinit_completion(&mcspi_dma->dma_rx_completion);
            pr_debug("@@@@ %s: no data, block %s\n", __func__,current->comm);
            ret = wait_for_completion_interruptible(&mcspi_dma->dma_rx_completion);
            if(ret){
                ret = -ERESTARTSYS;
                goto OUT;
            }
        }
    }
    tocpy = (cnt > dev_buf->prd_sz)? dev_buf->prd_sz : cnt;

    data_ptr = buf_addr_popup(dev_buf);
    if((data_ptr == NULL) || (tocpy < 0)){
        ret = -EFAULT;
        goto OUT;
    }

    ret = copy_to_user(usr_buf, data_ptr, tocpy);
    if(!ret)
        ret = tocpy;

OUT:
    return ret;
}

/*
 * first operation performed on device file...
 */
int slave_intf_open (struct inode * ind, struct file *filp)
{
    int ret = 0;

    spiATmegaVics_dev_t *vics_devp;
    omap2_slave_mcspi_t *slave;

#ifdef DEBUG_POLL_RX
    u32 reg_val;
#endif

    period_sz = get_period_sz();

    if(period_sz > PERIOD_SZ)
        period_sz = PERIOD_SZ;

    pr_info("@@@@ %s: use period_sz = %u \n", __func__, period_sz);

    vics_devp = container_of(ind->i_cdev, spiATmegaVics_dev_t, cdev);

    if (vics_devp == NULL)
        return -ENODEV;

    slave = vics_devp->slave;

    if (slave == NULL)
        return -ENODEV;

    if(!atomic_read(&vics_devp->state))
        return -EAGAIN;

    vics_devp->rd_buff = &slave->in_buff;
    filp->private_data = vics_devp;

    if(down_trylock(&vics_devp->dev_sem))
        goto dev_busy;

    flush_buf(&slave->in_buff);
    slave->in_buff.prd_sz = period_sz;

    omap2_slave_mcspi_fifo_enable(slave, true);
    omap2_slave_mcspi_chn_enable(slave, true);

#ifdef DEBUG_POLL_RX
    while(1){

        do{
            mdelay(100);
            reg_val = mcspi_read_reg(slave, OMAP2_MCSPI_CHSTAT0);
         }while(!(reg_val & OMAP2_MCSPI_CHSTAT_RXS));

        printk(KERN_ALERT "@@@@ %s: recvd %02x\n", __func__,
                        mcspi_read_reg(slave, OMAP2_MCSPI_RX0));
    }
#endif

    ret = omap2_slave_mcspi_setup_rxdma (slave);
    if(ret < 0)
        goto out_err;

    omap2_slave_mcspi_set_dma_req(slave, 1, true);

    return 0;

dev_busy:
    ret = -EBUSY;
    pr_info("@@@@ %s: Dude device busy.. best luck next time\n", __func__);

out_err:
    return ret;
}

/*
 * close (cleanup) operation performed on device file...
 */
int slave_intf_close (struct inode * ind, struct file *file)
{
    spiATmegaVics_dev_t *vics_devp;
    omap2_slave_mcspi_t *slave;
    struct omap2_mcspi_dma *mcspi_dma;

    pr_debug("@@@@ %s \n", __func__);

    vics_devp = container_of(ind->i_cdev, spiATmegaVics_dev_t, cdev);
    slave = vics_devp->slave;
    mcspi_dma = &slave->dma_channel;

    /*DISABLE SPI, and DMA, cleanup queue too?? */
    
    omap2_slave_mcspi_fifo_enable(slave, false);
    omap2_slave_mcspi_chn_enable(slave, false);

    omap2_slave_mcspi_set_dma_req(slave, 1, false);
    dmaengine_terminate_all(mcspi_dma->dma_rx);

    pr_debug("@@@@ %s: %s, clear busy flag on device\n",
                                            __func__, current->comm);
    up(&vics_devp->dev_sem);

    return 0;
}


struct file_operations slave_intrf_ops = {
    .owner = THIS_MODULE,
    .open  = slave_intf_open,
    .read  = slave_intf_read,
    .write = slave_intf_write,
    .release = slave_intf_close,
};

static struct omap2_mcspi_platform_config omap2_pdata = {
    .regs_offset = 0,
};

static struct omap2_mcspi_platform_config omap4_pdata = {
    .regs_offset = OMAP4_MCSPI_REG_OFFSET,
};

static const struct of_device_id omap_slave_mcspi_of_match[] = {
    {
        .compatible = "ti,omap2-slave-mcspi",
        .data = &omap2_pdata,
    },
    {
        .compatible = "ti,omap4-slave-mcspi",
        .data = &omap4_pdata,
    },
    { },
};
MODULE_DEVICE_TABLE(of, omap_slave_mcspi_of_match);

#if 0
static void spi_slave_release(struct device *dev)
{
        omap2_slave_mcspi_t *slave;

        slave = container_of(dev, omap2_slave_mcspi_t, dev);
        kfree(slave);
}

static struct class spi_slave_class = {
        .name           = "spi_slave",
        .owner          = THIS_MODULE,
        .dev_release    = spi_slave_release,
};
#endif

static int omap2_slave_mcspi_setup ( omap2_slave_mcspi_t *slave)
{
    uint32_t reg_val =  mcspi_read_reg(slave, OMAP2_MCSPI_CHCONF0);

    pr_debug("@@@@ %s...\n", __func__);

    if(slave->mode & SPI_CPOL){
        reg_val |= OMAP2_MCSPI_CHCONF_POL;
        pr_info("@@@@ %s: SPICLK - HIGH during inactive state\n", __func__ );
    }
    else{
        reg_val &= ~(OMAP2_MCSPI_CHCONF_POL);
        pr_info("@@@@ %s: SPICLK - LOW during inactive state\n", __func__ );
    }
    
    if(slave->mode & SPI_CPHA){
        reg_val |= OMAP2_MCSPI_CHCONF_PHA;
        pr_info("@@@@ %s: data latched on even-numbered edges\n", __func__ );
    }
    else{
        reg_val &= ~(OMAP2_MCSPI_CHCONF_PHA);
        pr_info("@@@@ %s: data latched on even-numbered edges\n", __func__ );
    }

    pr_info("@@@@ %s: SPIEN-CS- LOW  during active state\n", __func__);
    reg_val |= OMAP2_MCSPI_CHCONF_EPOL;


    if(slave->pin_dir == MCSPI_PINDIR_D0_IN_D1_OUT){
        reg_val &= ~OMAP2_MCSPI_CHCONF_IS;
        reg_val &= ~OMAP2_MCSPI_CHCONF_DPE1;
        reg_val |= OMAP2_MCSPI_CHCONF_DPE0;
        pr_info("@@@@ %s: DAT[0] line as Rx line\n", __func__);
        pr_info("@@@@ %s: DAT[1] line as Tx line\n", __func__);
    }
    else {
        reg_val |= OMAP2_MCSPI_CHCONF_IS;
        reg_val |= OMAP2_MCSPI_CHCONF_DPE1;
        reg_val &= ~OMAP2_MCSPI_CHCONF_DPE0;
        pr_info("@@@@ %s: DAT[1] line as Rx line\n", __func__);
        pr_info("@@@@ %s: DAT[0] line as Tx line\n", __func__);
    }

    /* word-len... need to be set */
    reg_val &= ~(OMAP2_MCSPI_CHCONF_WL_MASK);
    reg_val |= (slave->word_len - 1) << 7;
    pr_info("@@@@ %s: SPI word len %d bits long\n", __func__, slave->word_len);

    pr_debug("@@@@ %s : SPI CLK freq controlled by master..."\
            " nothing to be done from our side\n", __func__);

    /* apply above configurations....*/
    mcspi_write_reg(slave, OMAP2_MCSPI_CHCONF0, reg_val);

    return 0;

}

static int omap2_slave_mcspi_config ( omap2_slave_mcspi_t *slave)
{
    int ret = 0;
    uint32_t reg_val;

    pr_debug("@@@@ %s...\n", __func__);

    ret = pm_runtime_get_sync(slave->dev);
    if(ret < 0)
        return ret;

    /* start from know condition, reset controller */
    reg_val = mcspi_read_reg(slave, OMAP2_MCSPI_SYSCONFIG);
    reg_val |= OMAP2_MCSPI_SYSCONFIG_RST;
    mcspi_write_reg(slave, OMAP2_MCSPI_SYSCONFIG, reg_val);

    reg_val = 0;

    do{
        mdelay(100);
        reg_val = mcspi_read_reg(slave, OMAP2_MCSPI_SYSSTATUS);
    }while(!reg_val);
        
    /* select slave mode */
    reg_val = mcspi_read_reg(slave, OMAP2_MCSPI_MODULCTRL);
    reg_val |=  OMAP2_MCSPI_MODULCTRL_MS;
    mcspi_write_reg(slave, OMAP2_MCSPI_MODULCTRL, reg_val);

    /* slave select signal detection on cs0..
     * TODO : based on DTS entry...
     */
    reg_val = mcspi_read_reg(slave, OMAP2_MCSPI_CHCONF0);
    reg_val &= ~(0x3 << 21);
    mcspi_write_reg(slave, OMAP2_MCSPI_CHCONF0, reg_val);

    /* empty tx register... TX0 register will be loaded 
     * into the shift register, when external master selects mcspi
     */
    mcspi_write_reg(slave, OMAP2_MCSPI_TX0, 0);

    return 0;
}

/*
 * Runs in interrupt context...
 * update fifo write index..
 */
static void omap2_slave_mcspi_rx_callback(void *data)
{
    omap2_slave_mcspi_t *slave = (omap2_slave_mcspi_t *) data;
    circ_buff_t *in_buf = &slave->in_buff;
    struct omap2_mcspi_dma *mcspi_dma = &slave->dma_channel;

    dbg_print("XXXX %s: wr indx %d\n",\
                 __func__, in_buf->wr_indx/in_buf->prd_sz);

    in_buf->wr_indx += in_buf->prd_sz;

    if(in_buf->wr_indx >= in_buf->buf_sz)
        in_buf->wr_indx = 0;

    dbg_print("XXXX %s: wr indx %d\n",\
                 __func__, in_buf->wr_indx/in_buf->prd_sz);

    /* TODO: wake up sleepers. etc... */
    complete(&mcspi_dma->dma_rx_completion);

    return;
}


static int omap2_slave_mcspi_setup_rxdma ( omap2_slave_mcspi_t *slave)
{
    struct dma_slave_config cfg;
    struct omap2_mcspi_dma *mcspi_dma;
    struct dma_async_tx_descriptor *rx;

    enum dma_slave_buswidth width = 0;
    uint32_t  burst = DMA_SLAVE_BUSWIDTH_UNDEFINED;
    uint32_t es = 1;

    pr_debug("@@@@ %s...\n", __func__);

    mcspi_dma = &slave->dma_channel;

    if(slave->word_len <= 8){
        width = DMA_SLAVE_BUSWIDTH_1_BYTE;
        es = 1;
    } else if (slave->word_len <=16){
        width = DMA_SLAVE_BUSWIDTH_2_BYTES;
        es = 2;
    } else {
        width = DMA_SLAVE_BUSWIDTH_4_BYTES;
        es = 4;
    }

    if(period_sz > slave->fifo_depth)
        burst = slave->fifo_depth / es;
    else
        burst = period_sz/es; 

    /* TODO: debug why burst size > 1 not working?
     * burst = 8;
     */
    burst = 1;

    memset(&cfg, 0, sizeof(cfg));
    cfg.src_addr = slave->phys + OMAP2_MCSPI_RX0;
    cfg.dst_addr = slave->phys + OMAP2_MCSPI_TX0;
    cfg.src_addr_width = width;
    cfg.dst_addr_width = width;
    cfg.src_maxburst = burst;
    cfg.dst_maxburst = burst;

    pr_info("@@@@ %s: width %d, burst %d\n", __func__, width, burst);

    if(!mcspi_dma->dma_rx) {
        pr_err("XXXX @%s, oops rx channel not available\n", __func__);
        return -ENODEV;
    }

    dmaengine_slave_config(mcspi_dma->dma_rx, &cfg);

    rx = dmaengine_prep_dma_cyclic( mcspi_dma->dma_rx, slave->in_bus_addr,
                                    buf_sz, period_sz, DMA_DEV_TO_MEM, 
                                    DMA_PREP_INTERRUPT | DMA_CTRL_ACK);

    if(rx) {
        rx->callback = omap2_slave_mcspi_rx_callback;
        rx->callback_param = slave;
        dmaengine_submit(rx);
    }
    else {
        pr_err("@@@@ %s... failed to prep dma\n", __func__);
        /*TODO: fallback to PIO */
    }
    dma_async_issue_pending(mcspi_dma->dma_rx);

    return 0;
}

static void omap2_slave_mcspi_set_dma_req(omap2_slave_mcspi_t *slave,
                        int is_read, int enable)
{
    uint32_t reg_val, rw;

    pr_debug("@@@@ %s...\n", __func__);

    reg_val = mcspi_read_reg(slave, OMAP2_MCSPI_CHCONF0);

    if( is_read ){
        pr_info("@@@@ %s: request for read DMA, for new Rx data\n", __func__);
        rw = OMAP2_MCSPI_CHCONF_DMAR;
    }
    else
        rw = OMAP2_MCSPI_CHCONF_DMAW;

    if(enable){
        pr_info("@@@@ %s: Enable DMA request...\n", __func__);
        reg_val |= rw;
    }
    else{
        pr_info("@@@@ %s: Disable DMA request...\n", __func__);
        reg_val &= ~rw;
    }

    mcspi_write_reg(slave, OMAP2_MCSPI_CHCONF0, reg_val);
    return;    
}
    
static int omap2_slave_mcspi_request_dma(omap2_slave_mcspi_t *slave)
{
    struct omap2_mcspi_dma *mcspi_dma;
    dma_cap_mask_t mask;
    uint32_t sig;
    int ret = 0;

    pr_debug("@@@@ %s...\n", __func__);

    mcspi_dma = &slave->dma_channel;

    init_completion(&mcspi_dma->dma_rx_completion);
    init_completion(&mcspi_dma->dma_tx_completion);

    dma_cap_zero(mask);
    dma_cap_set(DMA_SLAVE, mask);

    sig = mcspi_dma->dma_rx_sync_dev;

    mcspi_dma->dma_rx = 
            dma_request_slave_channel_compat(mask, omap_dma_filter_fn,
                                             &sig, slave->dev,
                                             mcspi_dma->dma_rx_ch_name);

    if(!mcspi_dma->dma_rx){
        ret = -EAGAIN;
        pr_err("@@@@ %s: failed to get dma channel\n", __func__);
    }
    dbg_print("@@@@ %s: got dma channel for Rx- %d\n",
                 __func__, mcspi_dma->dma_rx->chan_id);


    /*TODO: get for tx ?? not required at this point of time...*/

    return 0;
}


static int omap2_slave_mcspi_probe(struct platform_device *pdev)
{
    const struct omap2_mcspi_platform_config *pdata;
    struct resource         *r;
    struct resource         *dma_res;
    u32                     regs_offset = 0;
    struct device_node      *node = pdev->dev.of_node;
    const struct of_device_id *match;
   
    struct omap2_mcspi_dma *mcspi_dma;

    circ_buff_t *in_buf;
    circ_buff_t *out_buf;

    char * dma_rx_ch_name;
    char * dma_tx_ch_name;
    void *rdbuf_ptr;

    int status = 0, i = 0;
 
    pr_debug("@@@@ %s\n", __func__);

    slave = kmalloc( sizeof(*slave), GFP_KERNEL);

    if(slave == NULL){
        pr_err("@@@@ %s: Failed to allocate memory to slave cookie\n",
                                                                __func__);
        return -1;
    }
    vics_dev.slave = slave;

    memset(slave, 0, sizeof(*slave));

    in_buf = &slave->in_buff;
    out_buf = &slave->out_buff;

#if 0
    device_initialize(&slave->dev);
    slave->dev.class = &spi_slave_class;
    slave->dev.parent = get_device(&pdev->dev);
    slave->dev.of_node = node;
#endif

    slave->bus_nr = -1;
    platform_set_drvdata(pdev, slave);


    match = of_match_device(omap_slave_mcspi_of_match, &pdev->dev);
    if(match){
        u32 num_cs = 1;
        u32 pin_dir = 1;
        pdata = match->data;

        of_property_read_u32(node,  "ti,spi-num-cs", &num_cs);
        slave->num_chipselect = num_cs;
        pr_info("@@@@ %s: chipselect num %#x\n", __func__, num_cs);

        of_property_read_u32(node, "ti,pindir-d0-out-d1-in", &pin_dir);
        slave->pin_dir = pin_dir;
        pr_info("@@@@ %s: d0-d1 dir %#x\n", __func__, slave->pin_dir);
    }
    else{
        pdata = dev_get_platdata(&pdev->dev);
        slave->num_chipselect = pdata->num_cs;
        slave->pin_dir = pdata->pin_dir;
    }
    if (pdev->id != -1)
        slave->bus_nr = pdev->id;

    /* TODO: get from dts... */
    slave->word_len = 8;
    slave->mode = SPI_MODE_1;
    slave->fifo_depth = OMAP2_MCSPI_MAX_FIFODEPTH / 2 ;

    regs_offset = pdata->regs_offset;

    dbg_print("@@@@ %s... use regs offset %#x\n", __func__, regs_offset);

    r = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if(r == NULL){
        status = -ENODEV;
        goto free_slave;
    }

    r->start += regs_offset;
    r->end += regs_offset;

    pr_info("@@@@ %s: base address for MCSPI cntrl %#x\n",
                                                 __func__, r->start);
    /* for book-keeping purpose */
    slave->phys =  r->start;

    slave->base = devm_ioremap_resource(&pdev->dev, r);
    if(IS_ERR(slave->base)){
        pr_err("@@@@ %s: failed to map MEM resource...\n", __func__);
        status = PTR_ERR(slave->base);
        goto free_slave;
    }

    dma_rx_ch_name = slave->dma_channel.dma_rx_ch_name;
    dma_tx_ch_name = slave->dma_channel.dma_tx_ch_name;
    mcspi_dma = &slave->dma_channel;

    sprintf(dma_rx_ch_name, "rx%d", i);
    if(!pdev->dev.of_node){
        dma_res = platform_get_resource_byname(pdev, 
                                               IORESOURCE_DMA,
                                               dma_rx_ch_name);
        if(!dma_res){
            pr_debug("@@@@ %s: cannot get DMA RX channel", __func__);
            status = -ENODEV;
            goto free_slave;
        }
        slave->dma_channel.dma_rx_sync_dev = dma_res->start;
        pr_info("@@@@ %s: rx_sync_dev %#x\n", __func__, dma_res->start);
    }

    sprintf(dma_tx_ch_name, "tx%d", i);
    if(!pdev->dev.of_node){
        dma_res = platform_get_resource_byname(pdev, 
                                               IORESOURCE_DMA,
                                               dma_tx_ch_name);
        if(!dma_res){
            pr_debug("@@@@ %s: cannot get DMA TX channel", __func__);
            status = -ENODEV;
            goto free_slave;
        }
        slave->dma_channel.dma_tx_sync_dev = dma_res->start;
        pr_info("@@@@ %s: tx_sync_dev %#x\n", __func__, dma_res->start);
    }

    slave->dev = &pdev->dev;

    if(!dma_set_mask(slave->dev, DMA_BIT_MASK(64))) {
        dma_set_coherent_mask(slave->dev, DMA_BIT_MASK(64));
    }
    else if(!dma_set_mask(slave->dev, DMA_BIT_MASK(32))) {
        dma_set_coherent_mask(slave->dev, DMA_BIT_MASK(64));
    }
    else {
        pr_err("@@@@ %s: non supported dma address range\n", __func__);
        goto free_slave;
    }

    /* Allocate buffer... */
    memset(in_buf, 0, sizeof(*in_buf));
    spin_lock_init(&in_buf->cb_lock);
    
    rdbuf_ptr = dma_alloc_coherent(slave->dev, BUFFER_SZ,
                                            &slave->in_bus_addr,
                                            GFP_KERNEL | GFP_DMA);

    if(!rdbuf_ptr){
        pr_err("@@@@ %s: failed to get coherent memory chunk\n", __func__);
        goto free_slave;
    }
    init_buf(in_buf, rdbuf_ptr, buf_sz, period_sz, periods);

    pm_runtime_enable(slave->dev);
    status = pm_runtime_get_sync(slave->dev);
    if(status < 0)
        goto free_dma;
   
    if(!mcspi_dma->dma_rx || !mcspi_dma->dma_tx){
        status = omap2_slave_mcspi_request_dma(slave);
        if(status < 0)
            goto free_dma;
    }

    omap2_slave_mcspi_config(slave);    
    omap2_slave_mcspi_chn_enable(slave, false);
    omap2_slave_mcspi_setup(slave);

    /* initalization done */
    atomic_set(&vics_dev.state, 1);
    return 0;

free_dma:
    dma_free_coherent(slave->dev, BUFFER_SZ, in_buf->buf_ptr,
                        slave->in_bus_addr); 
free_slave:
    kfree(slave);

    return status;
}

static int omap2_slave_mcspi_remove(struct platform_device *pdev)
{
    omap2_slave_mcspi_t *slave;
    circ_buff_t *in_buf;

    slave = platform_get_drvdata(pdev); 
    in_buf = &slave->in_buff;

    dma_free_coherent(slave->dev, BUFFER_SZ, in_buf->buf_ptr,
                                            slave->in_bus_addr); 
    kfree(slave);

    /* device not initalized/ accessable.. */
    atomic_set(&vics_dev.state, 0);

    return 0;
}


static const struct dev_pm_ops omap2_mcspi_pm_ops = {
    .resume = NULL,
    .runtime_resume = NULL,
};

static struct platform_driver omap2_slave_mcspi_driver = {
    .driver = {
        .name  =   "omap2_slave_mcspi",
        .owner =   THIS_MODULE,
        .pm    =   &omap2_mcspi_pm_ops,
        .of_match_table = omap_slave_mcspi_of_match,
    },

    .probe = omap2_slave_mcspi_probe,
    .remove = omap2_slave_mcspi_remove,
};


static int __init mcspi_omap24xx_slave_init(void)
{
    int ret;
    dev_t *dev_nr;

    memset(&vics_dev, 0, sizeof(vics_dev));
    dev_nr = &(vics_dev.dev_nr);

    init_MUTEX(&vics_dev.dev_sem);
    atomic_set(&vics_dev.state, 0);

    /* Time to hook-up with hardware */
    ret = platform_driver_register(&omap2_slave_mcspi_driver);

    if(ret){
        pr_err("@@@@ %s : Failed to register platform driver\n", __func__);
        return ret;
    }

    /* register character deivce.... */
    if(major){
        *dev_nr = MKDEV(major, DARC_DATA_DEV);
        ret = register_chrdev_region(*dev_nr, DEVICES, DEV_NAME);
    }
    else{
        ret = alloc_chrdev_region(dev_nr, DARC_DATA_DEV, DEVICES, DEV_NAME);
    }

    if(ret !=0)
        return ret;

    /* register character driver interface for vics.. */
    cdev_init(&vics_dev.cdev, &slave_intrf_ops );
    ret = cdev_add(&vics_dev.cdev, *dev_nr, DEVICES);

    if(ret < 0)
        return ret;

    /* Main reason to have class created is to make mdev/udev create 
     * character device node exposing posixs API.
     */
    vics_dev.class = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(vics_dev.class)){
        pr_err("@@@@ %s: Failed to create device class %s\n",
                                            __func__, CLASS_NAME);
        pr_err("@@@@ %s: Manually create device node\n",__func__);
    }
    else{
        vics_dev.dev = device_create( vics_dev.class, NULL, 
                                        *dev_nr, NULL, DEV_NAME);
        if(IS_ERR(vics_dev.dev)){
            pr_err("@@@@ %s: Failed to create device %s\n",
                                             __func__, DEV_NAME);
            pr_err("@@@@ %s: Manually create device node\n",
                                                        __func__);
        }
        else {
            /*sysfs interface registration */
            kobject_init(&vics_dev.kobj, &slave_kobj_type);
            ret = kobject_add(&vics_dev.kobj, &vics_dev.dev->kobj,\
                                                      "%s", "params");
            if(ret < 0)
                kobject_put(&vics_dev.kobj);
        }
    }

    return 0;
}

static void __exit mcspi_omap24xx_slave_exit(void)
{
    /* unregister platform driver...*/
    platform_driver_unregister(&omap2_slave_mcspi_driver);
    /* unregister character device... */
    cdev_del(&vics_dev.cdev);
    unregister_chrdev_region(vics_dev.dev_nr, DEVICES);

    return;
}

module_init(mcspi_omap24xx_slave_init);
module_exit(mcspi_omap24xx_slave_exit);

MODULE_DESCRIPTION("MCSPI controller configured in slave mode");
MODULE_VERSION("0.4");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Padmanabha S");

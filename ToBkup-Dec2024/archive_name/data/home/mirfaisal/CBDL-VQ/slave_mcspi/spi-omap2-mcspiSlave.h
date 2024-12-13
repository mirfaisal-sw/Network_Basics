#ifndef __SPI_OMAP2_MCSPISLAVE_H
#define __SPI_OMAP2_MCPSISLAVE_H

/* Need Macro defintions for clock, mode, and etc... */

#define OMAP2_MCSPI_MAX_FIFODEPTH           64

#define OMAP2_MCSPI_REVISION                0x00
#define OMAP2_MCSPI_SYSCONFIG               0x10
#define OMAP2_MCSPI_SYSSTATUS               0x14
#define OMAP2_MCSPI_IRQSTATUS               0x18
#define OMAP2_MCSPI_IRQENABLE               0x1c
#define OMAP2_MCSPI_WAKEUPENABLE            0x20
#define OMAP2_MCSPI_SYST                    0x24
#define OMAP2_MCSPI_MODULCTRL               0x28
#define OMAP2_MCSPI_XFERLEVEL               0x7c
#define OMAP2_MCSPI_DFRAX                   0xA0

/* per-channel banks, 0x14 bytes each, first is: */
#define OMAP2_MCSPI_CHCONF0                 0x2c
#define OMAP2_MCSPI_CHSTAT0                 0x30
#define OMAP2_MCSPI_CHCTRL0                 0x34
#define OMAP2_MCSPI_TX0                     0x38
#define OMAP2_MCSPI_RX0                     0x3c

/* per-register bitmasks: */
#define OMAP2_MCSPI_IRQSTATUS_EOW           BIT(17)

#define OMAP2_MCSPI_MODULCTRL_SINGLE        BIT(0)
#define OMAP2_MCSPI_MODULCTRL_MS            BIT(2)
#define OMAP2_MCSPI_MODULCTRL_STEST         BIT(3)

#define OMAP2_MCSPI_CHCONF_PHA              BIT(0)
#define OMAP2_MCSPI_CHCONF_POL              BIT(1)
#define OMAP2_MCSPI_CHCONF_EPOL             BIT(6)
#define OMAP2_MCSPI_CHCONF_WL_MASK          (0x1f << 7)
#define OMAP2_MCSPI_CHCONF_TRM_RX_ONLY      BIT(12)
#define OMAP2_MCSPI_CHCONF_TRM_TX_ONLY      BIT(13)
#define OMAP2_MCSPI_CHCONF_TRM_MASK         (0x03 << 12)
#define OMAP2_MCSPI_CHCONF_DMAW             BIT(14)
#define OMAP2_MCSPI_CHCONF_DMAR             BIT(15)
#define OMAP2_MCSPI_CHCONF_DPE0             BIT(16)
#define OMAP2_MCSPI_CHCONF_DPE1             BIT(17)
#define OMAP2_MCSPI_CHCONF_IS               BIT(18)
#define OMAP2_MCSPI_CHCONF_FORCE            BIT(20)
#define OMAP2_MCPSI_CHCONF_SPIENSLV_MASK    (0x03 << 21)
#define OMAP2_MCSPI_CHCONF_FFET             BIT(27)
#define OMAP2_MCSPI_CHCONF_FFER             BIT(28)


#define OMAP2_MCSPI_CHSTAT_RXS              BIT(0)
#define OMAP2_MCSPI_CHSTAT_TXS              BIT(1)
#define OMAP2_MCSPI_CHSTAT_EOT              BIT(2)
#define OMAP2_MCSPI_CHSTAT_RXFFE            BIT(3)

#define OMAP2_MCSPI_CHCTRL_EN               BIT(0)

#define OMAP2_MCSPI_SYSCONFIG_RST           BIT(1)

/* Frames.. periods for vics decoder */
#define FRAME_SZ    28
#define PERIOD_SZ   (FRAME_SZ * 8)
#define PERIODS     20
#define BUFFER_SZ   (PERIODS * PERIOD_SZ)

/* cookie to implement rd/wr device memory */
typedef struct circ_buff {
    void *buf_ptr;
    uint32_t rd_indx;
    uint32_t wr_indx;

    uint32_t prd_sz;
    uint32_t prds;
    uint32_t buf_sz;

    spinlock_t cb_lock;    /* gaurd IRQ context and process context */
}circ_buff_t;


/* 2 DMA channels per CS, one for RX and one for TX */
struct omap2_mcspi_dma {
    struct dma_chan *dma_tx;
    struct dma_chan *dma_rx;

    int dma_tx_sync_dev;
    int dma_rx_sync_dev;

    struct completion dma_tx_completion;
    struct completion dma_rx_completion;

    char dma_rx_ch_name[14];
    char dma_tx_ch_name[14];
};

/*
 * Used for context save and restore, structure members to be updated whenever
 * corresponding registers are modified.
 */
struct omap2_mcspi_regs {
    u32 modulctrl;
    u32 wakeupenable;
    struct list_head cs;
};


typedef struct omap2_mcspi_slave {
    /* Virtual based address of the controller */
    void __iomem            *base;
    uint32_t                phys;

    /* In slave configuration, can use channel 0 only */
    struct omap2_mcspi_dma  dma_channel;
    struct device           *dev;

    struct omap2_mcspi_regs ctx;
    int                     fifo_depth;

    circ_buff_t             in_buff;
    dma_addr_t              in_bus_addr;

    circ_buff_t             out_buff; 
    dma_addr_t              out_bus_addr;

    int32_t                 irq_line;
    int32_t                 rst_line;

    uint8_t                 mode;
    uint8_t                 word_len;
    uint8_t                 endian;
    uint8_t                 bus_nr;
    uint16_t                num_chipselect;
    uint32_t                pin_dir:1;

#define SPI_CPHA            0x1                /*clock phase */
#define SPI_CPOL            0x2                /*clock polarity */
#define SPI_MODE_0          (0|0)              /*orginal microware */
#define SPI_MODE_1          (0|SPI_CPHA)
#define SPI_MODE_2          (SPI_POL|0)
#define SPI_MODE_3          (SPI_CPOL|SPI_SPI_CPHA)
#define SPI_CS_HIGH         0x04            /*chipselect active high*/
#define SPI_LSB_FIRST       0x8             /*per word bits-on-wire */
#define SPI_3WIRE           0x10            /*SI/SO signals shared */
#define SPI_LOOP            0x20            /*loopback mode */
#define SPI_NO_CS           0x40            /* 1 dev/bus, no chipselect */
#define SPI_READY           0x80            /* slave pulls low to pause */

} omap2_slave_mcspi_t;


#endif /* __SPI_OMAP2_MCSPISLAVE_H  */


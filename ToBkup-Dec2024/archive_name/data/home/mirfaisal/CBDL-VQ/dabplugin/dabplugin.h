/*****************************************************************
 * Project        Harman/Becker dev-spi
 * (c) copyright  2008-2010
 * Company        Harman/Becker Automotive Systems GmbH
 *                All rights reserved
 *****************************************************************/
/**
 * @file          spi-multiclient-plugin.h
 * @ingroup       resource_manager
 * @author        Michael Hupfer
 * @date          28.04.2011
 * modified       28.04.2011
 *
 * @brief Contains data structures and function prototypes for IPC plugin
 */

#ifndef SPIMULTICLIENTPLUGIN_H_
#define SPIMULTICLIENTPLUGIN_H_

#include <linux/spi/spi.h>
#include <linux/mutex.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include "msgqueue.h"

#ifndef isWhitespace
#define isWhitespace(c) ((c) == '\n' || (c) == '\r' || (c) == '\t' || (c) == ' ')
#endif

#define DAB_MAX_SINGLE_PKT_SIZE 64    /* maximum packet size for a single transer to DAB */
#define DAB_MAX_IFS_GAP_DELAY     15100 /* inter-frame sequence delay in milliseconds */
#define DAB_MAX_PKT_SIZE          5472

/************************** Management part **************************/


/***************************************************/
/* struct mpc_device_t                              */
/***************************************************/
/** @brief Multiclient device data management structure
 * */
#define IGN_BUF_LEN        16
#define DAB_NUM_BOOT_MSG   2
#define DAB_MSG_HEADER_LEN 4

struct dab_dev_attr {
	int major_num;
	struct device *dev;
	struct class *class;
};
typedef struct
{
   msgqueue_t          *q_toSend;                                  /**< Queue of messages that should be sent */
   msgqueue_t          q_received;                                 /**< Queue of received messages */
   msgqueue_t          q_received_hdr;                             /**< Queue of received dab header messages */
   unsigned            qrecv_afl;      							   /**< Almost Full Level for the Receive queue */
   int                 rcvid;                                      /**< receive ID to reply to */
   uint8_t             *buffer;                                    /**< temp message buffer */
   uint8_t             *tx_buffer;                                 /**< temp TX message buffer */
   uint8_t             *rx_buffer;                                 /**< temp RX message buffer */
   uint8_t             dab_header_msg[DAB_MSG_HEADER_LEN];         /**< Dummer to use on read header on behalf of dab interrupt */
   uint8_t             *dab_dummy_msg;                             /**< Dummer to use on receiving dab interrupt */
   uint32_t            usn;
   uint32_t            last_usn;
   void                *plugin;
	int rx_pending;
	struct dab_dev_attr dev_attr;
} mc_device_t;

typedef struct
{
   uint8_t  mType;
   uint8_t  usn;
   uint16_t len;
}dab_header;
/****************************************************/
/* struct apix_gp_intr_t                                 */
/****************************************************/
/** @brief Data for throwing interrupts
 * */
typedef struct
{
   void*                      vbase;               /**< base for access */
   int                        dab_gpio_pin;         /**< dab read gpio pin number */
   struct sigevent            intrevent;            /**< event for signaling */
   int                        dab_intr_id;       /**< interrupt id */
   uint32_t                   gpioccbase;
} dab_gp_intr_t;
/****************************************************/
/* struct ipc_plugin_t                               */
/****************************************************/
/** @brief Main data management structure
 * */
#define DAB_APP_WRITE_ACTIVE  (1 << 0)
#define DAB_INT_READ_ACTIVE   (1 << 1)
#define DAB_APP_WRITE_NACTIVE 0

#define DAB_NO_CS_TOGGLE 1
#define DAB_TOGGLE_CS    0

#define MAX_NAME_SIZE 32
struct spi_dab_info {
	unsigned int fifo_depth, fifo_width, q_afl;
	unsigned int gpio_num, gpio_irq_num;
	char char_iface_name[20];
};



typedef struct
{
	struct spi_device	*spi;                           /**< Information coming from the SPI driver */
   mc_device_t                 devs;          /**< Array of devices */
   msgqueue_t                  q_toSend;                      /**< Queue of message that should be sent */
   msgqueue_t                  q_DummySend;                    /**< Queue of dummy apix message that should be sent */
	struct task_struct     *plugin_send_receive_threadid; /**< ID of send-receive thread */
   uint32_t                    control;                       /**< Member for flow control */
   dab_gp_intr_t               dab_attr;                      /**DAB(SAF3560) interrupt related attributes*/
	struct mutex             lock_data;                     /**< mutex for thread safety */
   struct completion          cond_data;                     /**< condvar for signaling new data */
   volatile unsigned long      op;                            /**< plugin operation - will be 1 if write */
   uint32_t                    hold_cs_actv;                  /**< this flag will be used by tx-rx thread to send devctl to csl */
   uint32_t                    gpiobase;
   uint32_t					   hw_type;
	struct spi_dab_info dab_info;
} mc_plugin_t;


/**
 * @brief Parses the configuration for one channel.
 * @param line -  config string
 * @param devname - place to store the device name
 * @param maxlen - maximum length of the device name
 * @param width - place to store the FIFO width
 * @param depth - place to store the FIFO depth
 * @returns EOK / error number
 */
int plugin_parse_device(char *line, char *devname, unsigned maxlen, unsigned *width, unsigned *depth, unsigned *q_afl);


int validateChckSum(const unsigned char * cmdBuf);

void dumpbuffer_plugin(const uint8_t *buffer, unsigned bsize, unsigned int pktlen);


#endif /*SPIMULTICLIENTPLUGIN_H_*/

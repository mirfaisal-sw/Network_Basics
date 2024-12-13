/*****************************************************************************
 * Project Harman Car Multimedia System
 * (c) copyright 2019
 * Harman International Industries, Incorporated
 * All rights reserved
 * Secrecy Level STRICTLY CONFIDENTIAL
 *
 * @file         ipc_debug.c
 * @ingroup      FS3IPC Component
 * @author       David Rogala <david.rogala@harman.com>
 *
 * This module contains handlers for the /dev/ipcdebug interface
 *****************************************************************************/

/*****************************************************************************
 * INCLUDES
 *****************************************************************************/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fcntl.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/types.h>

#include "log.h"
#include "fs3ipc_cfg.h"
#include "fs3ipc.h"
#include "fs3ipc_app.h"
#include "ipc.h"

#ifdef CONFIG_FS3IPC_DEBUG

/*****************************************************************************
 * DEFINES
 *****************************************************************************/
#define DEBUG_READ_BUF_SIZE 16384
#define DEBUG_WRITE_BUF_SIZE 768
#define CHAN_STATS_LEN 117
#define CHAN_STATS_DATA_LEN 104
#define CHAN_STATS_NA 10
#define APP_STATS_LEN 190
#define HDLC_STATS_LEN 217

/*****************************************************************************
 * DATA TYPES
 *****************************************************************************/

/*****************************************************************************
 * FUNCTION PROTOTYPES
 ****************************************************************************/

/*****************************************************************************
 * VARIABLES
 *****************************************************************************/
static char debugStr[DEBUG_READ_BUF_SIZE];

#ifdef CONFIG_FS3IPC_SIMULATE_TXRX

/*****************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************/
static signed char toHex(char hexValue)
{
	signed char toHexError;

	if ((hexValue >= '0') && hexValue <= '9')
		return (hexValue - '0');

	if ((hexValue >= 'a') && hexValue <= 'f')
		return (10 + (hexValue - 'a'));

	if ((hexValue >= 'A') && hexValue <= 'F')
		return (10 + (hexValue - 'A'));

	toHexError = 1;
	return -toHexError;
}

static void handle_simulate_txrx_error(const char *func, int ch,
				       const char *input)
{
	log_error("CH-%d: Invalid input string for %s(): %s", ch, func, input);
	log_error("Provide even no. of digits in hexadecimal. '0x' prefix is not required.");
	log_error("NB: Mixing alphabets(other than a-f, A-F) & digits can have undefined results.");
}

static int process_simulate_txrx(unsigned int instance, char *debug_buffer,
				 size_t count)
{
	int channel_num;
	const struct Tagfs3ipc_app_LTChannelCfgType *chan_cfg = NULL;
	const struct fs3ipc_app_ltconfig_s *app_cfg =
		&fs3ipc_app_appConfigs[instance];
	fs3ipc_u8 parsed_msg[FS3_IPC_MAX_MSG_SIZE];
	int i = 0;
	int32_t ret = count;

	if (sscanf(debug_buffer, "simulateRx(%d) ", &channel_num) == 1) {

		memset(&parsed_msg[0], 0, sizeof(parsed_msg));

		/* Find channel */
		for (i = 0; i < app_cfg->channelCount; i++) {
			if (app_cfg->ltChannelCfg[i] &&
				app_cfg->ltChannelCfg[i]->channel == channel_num) {
				log_info("SimulateRx: found channel %d ind %d", channel_num, i);
				chan_cfg = app_cfg->ltChannelCfg[i];
				break;
			}
		}

		/* Was the channel identified*/
		if (!chan_cfg) {
			log_error("SimulateRx: Invalid channel: %d", channel_num);
			ret = -EIO;
		} else {
			/* Parse message*/
			int buffer_size = 0;

			i = 0;
			/* Find ')'*/
			while (i < count) {
				if (debug_buffer[i] == ')') {
					i++;
					break;
				}
				i++;
			}

			/*
			 * Convert the passed ASCII to hex and put it into the message
			 * buffer
			 */

			while (i < count-1) {
				signed char c1, c2 = 0;

				c1 = toHex(debug_buffer[i]);
				c2 = toHex(debug_buffer[i+1]);

				if (c1 != -1 && c2 != -1) {
					parsed_msg[buffer_size++] =
						(((fs3ipc_u8)c1) << 4) + (fs3ipc_u8)c2;
					/* Read 2 chars. skip 1*/
					i += 2;
				} else {
					i++;
				}
			}

			if (buffer_size == 0) {
				ret = -EIO;
				handle_simulate_txrx_error(__func__, channel_num,
							   debug_buffer);
			} else {
				int qstat = 0;
				/* Post message to Rx queue */
				fs3ipc_os_GetResource(chan_cfg->RX_QLock);
				qstat = fs3ipc_app_QueuePush(chan_cfg->RX_Q, &parsed_msg[0],
						buffer_size);
				fs3ipc_os_ReleaseResource(chan_cfg->RX_QLock);

				if (fs3ipc_app_qStat_OK > qstat) {
					log_error("SimulateRx: Cannot queue chan %d msg; err %d",
						channel_num, qstat);
					ret = -EIO;
				}

				if (ret >= 0) {
					/* Wakeup any reader threads */
					fs3ipc_os_SetEvent(chan_cfg->client_cfg.osCfg,
						chan_cfg->client_cfg.rxEvent);

					LOG_DATA(1, &(parsed_msg[0]), buffer_size,
						FS3_IPC_MAX_MSG_SIZE,
						"Simulate IPC: Rx C%02d L%03d",
						channel_num, buffer_size);
				}
			}
		}
	} else if (sscanf(debug_buffer, "simulateTx(%d) ", &channel_num) == 1) {

		memset(&parsed_msg[0], 0, sizeof(parsed_msg));

		/* Find channel */
		for (i = 0; i < app_cfg->channelCount; i++) {
			if (app_cfg->ltChannelCfg[i] &&
				app_cfg->ltChannelCfg[i]->channel == channel_num) {
				log_info("SimulateTx: found channel %d ind %d", channel_num, i);
				chan_cfg = app_cfg->ltChannelCfg[i];
				break;
			}
		}

		/* Was the channel identified*/
		if (!chan_cfg) {
			log_error("SimulateTx: Invalid channel: %d", channel_num);
			ret = -EIO;
		} else {
			/* Parse message*/
			int buffer_size = 0;

			i = 0;
			/* Find ')'*/
			while (i < count) {
				if (debug_buffer[i] == ')') {
					i++;
					break;
				}
				i++;
			}

			/*
			 * Convert the passed ASCII to hex and put it into the message
			 * buffer
			 */
			while (i < count-1) {
				signed char c1, c2 = 0;

				c1 = toHex(debug_buffer[i]);
				c2 = toHex(debug_buffer[i+1]);

				if (c1 != -1 && c2 != -1) {
					parsed_msg[buffer_size++] =
						(((fs3ipc_u8)c1) << 4) + (fs3ipc_u8)c2;
					/* Read 2 chars. skip 1*/
					i += 2;
				} else {
					i++;
				}
			}

			if (buffer_size == 0) {
				ret = -EIO;
				handle_simulate_txrx_error(__func__, channel_num,
							   debug_buffer);
			} else {
				int qstat = 0;
				/* Post message to Rx queue */
				fs3ipc_os_GetResource(chan_cfg->TX_QLock);
				qstat = fs3ipc_app_QueuePush(chan_cfg->TX_Q, &parsed_msg[0],
						buffer_size);
				fs3ipc_os_ReleaseResource(chan_cfg->TX_QLock);

				if (fs3ipc_app_qStat_OK > qstat) {
					log_error("SimulateTx: Cannot queue chan %d msg; err %d",
						channel_num, qstat);
					ret = -EIO;
				}

				if (ret >= 0) {
					/* Wakeup any reader threads */
					fs3ipc_os_SetEvent(app_cfg->phys_txEventOsCfg,
						app_cfg->phys_txEvent);
					LOG_DATA(1, &(parsed_msg[0]), buffer_size,
						FS3_IPC_MAX_MSG_SIZE,
						"Simulate IPC: Tx C%02d L%03d",
						channel_num, buffer_size);
				}
			}
		}
	} else {
		ret = -EINVAL;
	}
	return ret;
}
#else
static inline int process_simulate_txrx(char *debug_buffer, size_t count)
{
	return -EINVAL;
}
#endif /* CONFIG_FS3IPC_SIMULATE_TXRX */

static ssize_t fs3ipc_debug_read(struct file *filp, char __user *buf,
				 size_t count, loff_t *f_pos)
{
	const fs3ipc_app_stats_t *appStats = NULL;
	const fs3ipc_hdlc_stats_t *hdlcStats = NULL;
	const fs3ipc_fc_chan_pbcfg_t *chanStats = NULL;
	const fs3ipc_app_ltcfg_t *cfg = NULL;
	const fs3ipc_app_ltchancfg_t *chanCfg = NULL;
	fs3ipc_u32 rx_pending = 0;
	fs3ipc_u32 tx_pending = 0;
	fs3ipc_u32 txq_free_bytes = 0;
	fs3ipc_u32 rxq_free_bytes = 0;
	fs3ipc_u32 rxq_size = 0;
	fs3ipc_u32 txq_size = 0;
	int channel = 0;
	int size;
	int i = 0;
	int read_bytes = 0;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;

	if (!filp || !filp->private_data) {
		log_error("NULL filp(%p) or NULL FS3IPC device", (void *)filp);
		return -EBADF;
	}

	instance = ((struct fs3ipc_uart *)filp->private_data)->fs3ipc_instance;
	appStats = fs3ipc_app_GetStats(instance);
	hdlcStats = fs3ipc_hdlc_GetStats(instance);
	cfg = &fs3ipc_app_appConfigs[instance];

	if (!buf || !f_pos) {
		log_error("[%u] Invalid arguments:- buf: %p, f_pos: %p", instance,
			  (void __user *)buf, (void *)f_pos);
		return -EINVAL;
	}

	size = strlen(debugStr);

	if (size <= 0) {
		size = snprintf(&debugStr[i], sizeof(debugStr), "FS3IPC CHAN:\n"
			"CHXX(TXSt|RXSt): TX - SentP|   QP|QFreeB/QSizeB|     SentB|LPB|MPB|Off / RX - RcvdP|   QP|QFreeB/QSizeB|     RcvdB|LPB| MPB|Off|Ovf\n");
		if (size < 0)
			log_error("CH stats: snprintf size=%d", size);
		else if (size < CHAN_STATS_LEN)
			log_error("CH stats: snprintf size=%d,expected=%d", size, CHAN_STATS_LEN);

		channel = FS3IPC_APP_FLOWCONTROL_CHAN;
		i = (size >= 0) ? (size + i) : (i);

		while (channel < cfg->channelCount) {
			chanStats = fs3ipc_app_GetChanStats(instance, channel);

			if (cfg->ltChannelCfg[channel]) {
				chanCfg = cfg->ltChannelCfg[channel];
				fs3ipc_get_rxq_msg_pending(chanCfg->client_cfg.handle,
							      &rx_pending);
				fs3ipc_get_txq_msg_pending(chanCfg->client_cfg.handle,
							      &tx_pending);
				fs3ipc_get_rxq_size(chanCfg->client_cfg.handle,
						    &rxq_size);
				fs3ipc_get_txq_size(chanCfg->client_cfg.handle,
						    &txq_size);
				fs3ipc_get_rxq_free_space(chanCfg->client_cfg.handle,
							  &rxq_free_bytes);
				fs3ipc_get_txq_free_space(chanCfg->client_cfg.handle,
							  &txq_free_bytes);
			}
			if (chanStats) {
				size = snprintf(&debugStr[i], sizeof(debugStr) - i,
					"CH%02d(%4s|%4s):    %7u|%5u|%6u/%6u|%10u|%3u|%3u|%3u /    %7u|%5u|%6u/%6u|%10u|%3u|%4u|%3u|%3s\n",
					channel, chanStats->txEnabled ? "XON" : "XOFF",
					chanStats->rxEnabled ? "XON" : "XOFF",
					chanStats->txCnt, tx_pending, txq_free_bytes, txq_size,
					chanStats->tx_byte_cnt, chanStats->largest_pkt_sent,
					chanStats->maxTxPerFrame, chanStats->tx_xoff_cnt,
					chanStats->rxCnt, rx_pending, rxq_free_bytes, rxq_size,
					chanStats->rx_byte_cnt, chanStats->largest_pkt_rcvd,
					cfg->flowControlCfg->ltStChanConfig[channel].maxRxPerFrame,
					chanStats->rx_xoff_cnt,
					chanStats->rxOverflow ? "T  " : "  F");
				if (size < 0) {
					log_error("CH stats data: snprintf size=%d", size);
				} else if (size < CHAN_STATS_DATA_LEN) {
					log_error("CH stats data: snprintf size=%d,expected=%d",
						  size, CHAN_STATS_DATA_LEN);
				}
				i = (size >= 0) ? (size + i) : (i);
			} else	{
				size = snprintf(&debugStr[i], sizeof(debugStr) - i,
						"CH%02d: N/A\n", channel);
				if (size < 0) {
					log_error("CH stats NA: snprintf size=%d", size);
				} else if (size < CHAN_STATS_NA) {
					log_error("CH stats NA: snprintf size=%d,expected=%d", size,
						  CHAN_STATS_NA);
				}
				i = (size >= 0) ? (size + i) : (i);
			}

			channel++;
		}

		size = snprintf(&debugStr[i], sizeof(debugStr) - i,
				"\nFS3IPC APP:\n"
				"WRAPPED MSG CNT: %10u| %10u| %10u\n"
				"RXLERR:          %10u\n"
				"RXCHERR:         %10u\n"
				"RXGENERR:        %10u| %10u\n"
				"BUFOFERR:        %10u\n\n",
				appStats->rxWrappedMsgCnt, appStats->txWrappedMsgCnt, appStats->rxWrappedMsgErrorCnt,
				appStats->rxMsgLengthErrorCnt,
				appStats->rxUnhandledChannelErrorCnt,
				appStats->rxGenErrorCnt, appStats->txGenErrorCnt,
				appStats->rxBufferOverflow);
		if (size < 0)
			log_error("APP stats: snprintf size=%d", size);
		else if (size < APP_STATS_LEN)
			log_error("APP stats: snprintf size=%d,expected=%d", size, APP_STATS_LEN);
		i = (size >= 0) ? (size + i) : (i);

		size = snprintf(&debugStr[i], sizeof(debugStr) - i,
				"FS3IPC HDLC:\n"
				"IC:    %10u| %10u\n"
				"SC:    %10u| %10u\n"
				"TR:    %10u\n"
				"RSeq:  %10u\n"
				"TR:    %10u\n"
				"RInv:  %10u\n"
				"RPol:  %10u\n"
				"RRst:  %10u\n"
				"CRC:   %10u\n"
				"TExp:  %10u\n",

				hdlcStats->TxIframeCnt, hdlcStats->RxIframeCnt,
				hdlcStats->TxSFrameCnt, hdlcStats->RxSFrameCnt,
				hdlcStats->TxRetransmit, hdlcStats->RxSequenceErrorCnt,
				hdlcStats->RxCrcErrorCnt, hdlcStats->RxInvalidFrameCnt,
				hdlcStats->RxPollReqCnt, hdlcStats->RxResetReqCnt,
				hdlcStats->RxCrcErrorCnt, hdlcStats->TxAckTimeout);
		if (size < 0)
			log_error("HDLC stats: snprintf size=%d", size);
		else if (size < HDLC_STATS_LEN)
			log_error("HDLC stats: snprintf size=%d,expected=%d", size, HDLC_STATS_LEN);
		i = (size >= 0) ? (size + i) : (i);

		size = strlen(debugStr);
	}

	if (size > 0 && *f_pos <= size) {
		read_bytes = min(count, (strlen(debugStr) - (int)*f_pos));

		if (read_bytes > 0) {
			if (copy_to_user(buf, &debugStr[*f_pos], read_bytes)) {
				log_error("copy_to_user() could not copy");
				return -EFAULT;
			}

			*f_pos += read_bytes;
		} else {
			read_bytes = 0;
		}

	} else {
		read_bytes = -1;
	}


	return read_bytes;
}

static ssize_t fs3ipc_debug_write(struct file *filp, const char __user *buf,
				  size_t count, loff_t *f_pos)
{
	int32_t ret = count;
	fs3ipc_StatusType ipcStat = fs3ipc_StatusType_OK;
	char debug_buffer[DEBUG_WRITE_BUF_SIZE];
	int channelNum;
	int log_level = 0;
	unsigned int instance = FS3IPC_NUM_OF_INSTANCES;

	if (!filp || !filp->private_data) {
		log_error("NULL filp(%p) or NULL FS3IPC device", (void *)filp);
		return -EBADF;
	}

	instance = ((struct fs3ipc_uart *)filp->private_data)->fs3ipc_instance;

	if (!buf || !count || count > sizeof(debug_buffer)) {
		log_error("[%u] Invalid arguments:- buf: %p, count: %zu(max: %zu)",
			  instance, (void __user *)buf, count,
			  sizeof(debug_buffer));
		return -EINVAL;
	}

	if (copy_from_user(debug_buffer, buf, count)) {
		log_error("[%u] Copying from user failed", instance);
		return -EFAULT;
	}

	if (ret >= 0) {
		if (sscanf(debug_buffer, "enableTrace(%d)", &channelNum) == 1) {

			ipcStat = fs3ipc_app_SetLogging(0, channelNum, FS3IPC_TRUE);

			if (ipcStat != fs3ipc_StatusType_OK) {
				log_error("Error %d enabling chan %d logging", ipcStat, channelNum);
				ret = -EIO;
			} else {
				log_info("enableTrace: channel %d", channelNum);
			}

		} else if (sscanf(debug_buffer, "disableTrace(%d)", &channelNum) == 1) {

			ipcStat = fs3ipc_app_SetLogging(0, channelNum, FS3IPC_FALSE);

			if (ipcStat != fs3ipc_StatusType_OK) {
				log_error("Error %d disabling chan %d logging", ipcStat, channelNum);
				ret = -EIO;
			} else {
				log_info("disableTrace: channel %d", channelNum);
			}

		} else if (sscanf(debug_buffer, "verbose(%d)", &log_level) == 1) {
			gIpcLogLevel = log_level;
			log_info("verbose: %d", log_level);
		} else if (strncmp(debug_buffer, "enableRawTrace()", sizeof("enableRawTrace()")-1) == 0) {
			fs3ipc_phys_setLogging(instance, FS3IPC_TRUE);
			log_info("enableRawTrace");
		} else if (strncmp(debug_buffer, "disableRawTrace()", sizeof("disableRawTrace()")-1) == 0) {
			fs3ipc_phys_setLogging(instance, FS3IPC_FALSE);
			log_info("disableRawTrace");
		} else if (strncmp(debug_buffer, "enableHdlcTrace()", sizeof("enableHdlcTrace()")-1) == 0) {
			fs3ipc_hdlc_setLogging(instance, FS3IPC_TRUE);
			log_info("enableHdlcTrace");
		} else if (strncmp(debug_buffer, "disableHdlcTrace()", sizeof("disableHdlcTrace()")-1) == 0) {
			fs3ipc_hdlc_setLogging(instance, FS3IPC_FALSE);
			log_info("disableHdlcTrace");
		} else {
			ret = process_simulate_txrx(instance, debug_buffer, count);
			if (ret == -EINVAL)
				log_error("Invalid debug command");
		}
	}

	return ret;
}

static __poll_t fs3ipc_debug_poll(struct file *filp, struct poll_table_struct *pt)
{
	return (EPOLLIN | EPOLLRDNORM);
}

static int fs3ipc_debug_close(struct inode *inode, struct file *filp)
{
	return 0;
}

static int fs3ipc_debug_open(struct inode *inode, struct file *filp)
{
	unsigned int minor = 0u;
	struct fs3ipc_uart *fs3ipc = NULL;

	if (!inode || !filp) {
		log_error("Invalid arguments:- inode: %p, filp: %p",
			  (void *)inode, (void *)filp);
		return -EBADF;
	}

	fs3ipc = container_of(inode->i_cdev, struct fs3ipc_uart, debug_cdev);
	minor = iminor(inode);
	if (fs3ipc->debug_ch != minor) {
		log_error("CH-%u: Channel number mismatch: %u", fs3ipc->debug_ch,
			  minor);
		return -EIO;
	}

	filp->f_pos = 0;
	filp->private_data = fs3ipc;
	debugStr[0] = '\0';
	log_debug("CH-%u: Opened the debug interface", minor);

	return 0;
}

const struct file_operations fs3ipc_debug_fops = {
	.owner = THIS_MODULE,
	.open = fs3ipc_debug_open,
	.read = fs3ipc_debug_read,
	.write = fs3ipc_debug_write,
	.poll = fs3ipc_debug_poll,
	.release = fs3ipc_debug_close,
};

#endif /* CONFIG_FS3IPC_DEBUG */

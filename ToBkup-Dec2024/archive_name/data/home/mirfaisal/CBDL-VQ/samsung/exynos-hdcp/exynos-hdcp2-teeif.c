// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2016 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <soc/samsung/exynos-smc.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <asm/cacheflush.h>
#include <linux/dma-mapping.h>

#include "exynos-hdcp2-teeif.h"
#include "exynos-hdcp2.h"
#include "exynos-hdcp2-log.h"

extern struct hdcp_ctx g_hdcp[MAX_NUM_OF_HDCP];
extern void __dma_inv_area(const void *start, size_t size);

static struct hci_ctx hctx[MAX_NUM_OF_HDCP] = {
{
	.msg = NULL,
	.state = HCI_DISCONNECTED
},
{
	.msg = NULL,
	.state = HCI_DISCONNECTED
},
};

int hdcp_tee_open(uint32_t node)
{
	int ret = 0;
	u64 phys_addr = 0;

	if (hctx[node].state == HCI_CONNECTED) {
		hdcp_info("HCI is already connected for node(%d)\n", node);
		return 0;
	}

	/* Allocate WSM for HDCP commnad interface */
	hctx[node].msg = (struct hci_message *)kzalloc(sizeof(struct hci_message), GFP_KERNEL);
	if (!hctx[node].msg) {
		hdcp_err("alloc wsm for HDCP node%d SWd is failed\n", node);
		return -ENOMEM;
	}

	/* send WSM address to SWd */
	phys_addr = virt_to_phys((void *)hctx[node].msg);

	ret = exynos_smc(SMC_HDCP_INIT, node, phys_addr, sizeof(struct hci_message));
	if (ret) {
		hdcp_err("Fail to set up connection of node%d with SWd. ret(%d)\n", node, ret);
		kfree(hctx[node].msg);
		hctx[node].msg = NULL;
		return -ECONNREFUSED;
	}

	hctx[node].state = HCI_CONNECTED;

	return 0;
}

int hdcp_tee_close(uint32_t node)
{
	int ret;

	if (hctx[node].state == HCI_DISCONNECTED) {
		hdcp_info("HCI is already disconnected for node%d\n", node);
		return 0;
	}

	kfree(hctx[node].msg);
	hctx[node].msg = NULL;

	/* send terminate command to SWd */
	ret = exynos_smc(SMC_HDCP_TERMINATE, 0, 0, 0);
	if (ret) {
		hdcp_err("HDCP: Fail to set up connection for node%d with SWd. ret(%d)\n",
			node, ret);
		return -EFAULT;
	}

	hctx[node].state = HCI_DISCONNECTED;

	return 0;
}

int hdcp_tee_comm(uint32_t node, struct hci_message *hci)
{
	int ret;

	if (!hci)
		return -EINVAL;

	/**
	 * kernel & TEE does not share cache.
	 * So, cache should be flushed before sending data to TEE
	 * and invalidate after come back to kernel
	 */
	dcache_clean_inval_poc((unsigned long)hci, (unsigned long)hci + sizeof(struct hci_message));
	dma_map_single(g_hdcp[node].dev, (void *)hci, sizeof(struct hci_message), DMA_TO_DEVICE);
	ret = exynos_smc(SMC_HDCP_PROT_MSG, node, 0, 0);

	dcache_inval_poc((unsigned long)hci, (unsigned long)hci + sizeof(struct hci_message));
	dma_map_single(g_hdcp[node].dev, (void *)hci, sizeof(struct hci_message), DMA_FROM_DEVICE);

	if (ret) {
		hdcp_info("SWd SMC_HDCP_PROT_MSG return(0x%x)\n", ret);
		return ret;
	}

	return 0;
}

int teei_gen_rtx(uint32_t node, uint32_t lk_type,
		uint8_t *rtx, size_t rtx_len,
		uint8_t *caps, uint32_t caps_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;

	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_GEN_RTX;
	hci->genrtx.lk_type = lk_type;
	hci->genrtx.len = rtx_len;
	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	/* check returned message from SWD */
	if (rtx && rtx_len)
		memcpy(rtx, hci->genrtx.rtx, rtx_len);
	if (caps && caps_len)
		memcpy(caps, hci->genrtx.tx_caps, caps_len);

	return ret;
}

int teei_get_txinfo(uint32_t node, uint8_t *version, size_t version_len,
		uint8_t *caps_mask, uint32_t caps_mask_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;

	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_GET_TXINFO;

	/* send command to swd */
	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	/* check returned message from SWD */
	memcpy(version, hci->gettxinfo.version, version_len);
	memcpy(caps_mask, hci->gettxinfo.caps_mask, caps_mask_len);

	return ret;
}

int teei_set_rxinfo(uint32_t node, uint8_t *version, size_t version_len,
		uint8_t *caps_mask, uint32_t caps_mask_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;

	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_SET_RXINFO;
	memcpy(hci->setrxinfo.version, version, version_len);
	memcpy(hci->setrxinfo.caps_mask, caps_mask, caps_mask_len);

	/* send command to swd */
	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	return ret;
}

int teei_verify_cert(uint32_t node, uint8_t *cert, size_t cert_len,
		uint8_t *rrx, size_t rrx_len,
		uint8_t *rx_caps, size_t rx_caps_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;
	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_VERIFY_CERT;
	hci->vfcert.len = cert_len;
	memcpy(hci->vfcert.cert, cert, cert_len);
	if (rrx && rrx_len)
		memcpy(hci->vfcert.rrx, rrx, rrx_len);
	if (rx_caps && rx_caps_len)
		memcpy(hci->vfcert.rx_caps, rx_caps, rx_caps_len);
	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	/* return verification result */
	return ret;
}

int teei_generate_master_key(uint32_t node, uint32_t lk_type, uint8_t *emkey, size_t emkey_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;
	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_GEN_MKEY;
	hci->genmkey.lk_type = lk_type;
	hci->genmkey.emkey_len = emkey_len;

	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	/* copy encrypted mkey & wrapped mkey to hdcp ctx */
	memcpy(emkey, hci->genmkey.emkey, hci->genmkey.emkey_len);

	/* check returned message from SWD */

	return 0;
}

int teei_set_rrx(uint32_t node, uint8_t *rrx, size_t rrx_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;

	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_SET_RRX;
	memcpy(hci->setrrx.rrx, rrx, rrx_len);

	/* send command to swd */
	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	return ret;
}

int teei_compare_ake_hmac(uint32_t node, uint8_t *rx_hmac, size_t rx_hmac_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;
	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_COMPARE_AKE_HMAC;
	hci->comphmac.rx_hmac_len = rx_hmac_len;
	memcpy(hci->comphmac.rx_hmac, rx_hmac, rx_hmac_len);

	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	return ret;
}

int teei_set_pairing_info(uint32_t node, uint8_t *ekh_mkey, size_t ekh_mkey_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;

	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_SET_PAIRING_INFO;
	memcpy(hci->setpairing.ekh_mkey, ekh_mkey, ekh_mkey_len);

	/* send command to swd */
	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	return ret;
}

int teei_get_pairing_info(uint32_t node, uint8_t *ekh_mkey, size_t ekh_mkey_len,
		uint8_t *m, size_t m_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;

	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_GET_PAIRING_INFO;

	/* send command to swd */
	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	memcpy(ekh_mkey, hci->getpairing.ekh_mkey, ekh_mkey_len);
	memcpy(m, hci->getpairing.m, m_len);

	return ret;
}

int teei_gen_rn(uint32_t node, uint8_t *out, size_t len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;

	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_GEN_RN;
	hci->genrn.len = len;

	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	/* check returned message from SWD */
	memcpy(out, hci->genrn.rn, len);

	return ret;
}

int teei_gen_lc_hmac(uint32_t node, uint32_t lk_type, uint8_t *lsb16_hmac)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;
	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_GEN_LC_HMAC;
	hci->genlchmac.lk_type = lk_type;

	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	/* Send LSB 16bytes to Rx */
	if (lsb16_hmac)
		memcpy(lsb16_hmac, hci->genlchmac.lsb16_hmac, (HDCP_HMAC_SHA256_LEN / 2));

	/* todo: check returned message from SWD */

	return 0;
}

int teei_compare_lc_hmac(uint32_t node, uint8_t *rx_hmac, size_t rx_hmac_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;
	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_COMPARE_LC_HMAC;
	hci->complchmac.rx_hmac_len = rx_hmac_len;
	memcpy(hci->complchmac.rx_hmac, rx_hmac, rx_hmac_len);

	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	return ret;
}

int teei_generate_riv(uint32_t node, uint8_t *out, uint8_t *typ, size_t len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;

	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_GEN_RIV;
	hci->genriv.len = len;

	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	memcpy(out, hci->genriv.riv, len);
	memcpy(typ, &hci->genriv.type, sizeof(uint8_t));

	/* todo:  check returned message from SWD */

	return ret;
}

int teei_generate_skey(uint32_t node, uint32_t lk_type,
		uint8_t *eskey, size_t eskey_len,
		int share_skey)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;
	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_GEN_SKEY;
	hci->genskey.lk_type = lk_type;
	hci->genskey.eskey_len = eskey_len;
	hci->genskey.share_skey = share_skey;

	ret = hdcp_tee_comm(node, hci);
	if (ret < 0)
		return ret;

	/* copy encrypted mkey & wrapped mkey to hdcp ctx */
	memcpy(eskey, hci->genskey.eskey, hci->genskey.eskey_len);

	/* todo: check returned message from SWD */

	return 0;
}

int teei_set_rcvlist_info(uint32_t node, uint8_t *rx_info,
		uint8_t *seq_num_v,
		uint8_t *v_prime,
		uint8_t *rcvid_list,
		uint8_t *v,
		uint8_t *valid)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;
	/* todo: input check */

	hci->cmd_id = HDCP_TEEI_SET_RCV_ID_LIST;

	memcpy(hci->setrcvlist.rcvid_lst, rcvid_list, HDCP_RP_RCVID_LIST_LEN);
	memcpy(hci->setrcvlist.v_prime, v_prime, HDCP_RP_HMAC_V_LEN / 2);
	/* Only used DP */
	if (rx_info != NULL && seq_num_v != NULL) {
		memcpy(hci->setrcvlist.rx_info, rx_info, HDCP_RP_RX_INFO_LEN);
		memcpy(hci->setrcvlist.seq_num_v, seq_num_v, HDCP_RP_SEQ_NUM_V_LEN);
	}


	ret = hdcp_tee_comm(node, hci);
	if (ret != 0) {
		*valid = 1;
		return ret;
	}
	memcpy(v, hci->setrcvlist.v, HDCP_RP_HMAC_V_LEN / 2);
	*valid = 0;

	return 0;
}


int teei_gen_stream_manage(uint32_t node, uint16_t stream_num,
		uint8_t *streamid,
		uint8_t *seq_num_m,
		uint8_t *k,
		uint8_t *streamid_type)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;
	/* todo: input check */

	/* Update TCI buffer */
	hci->cmd_id = HDCP_TEEI_GEN_STREAM_MANAGE;
	hci->genstrminfo.stream_num = stream_num;
	memcpy(hci->genstrminfo.k, k, HDCP_RP_K_LEN);
	memcpy(hci->genstrminfo.streamid_type, streamid_type,
		HDCP_RP_STREAMID_TYPE_LEN * stream_num);

	ret = hdcp_tee_comm(node, hci);

	/* return verification result */
	return ret;
}

int teei_verify_m_prime(uint32_t node, uint8_t *m_prime, uint8_t *input, size_t input_len)
{
	int ret = 0;
	struct hci_message *hci = hctx[node].msg;

	hci->cmd_id = HDCP_TEEI_VERIFY_M_PRIME;
	memcpy(hci->verifymprime.m_prime, m_prime, HDCP_RP_HMAC_M_LEN);
	if (input && input_len < sizeof(hci->verifymprime.strmsg)) {
		memcpy(hci->verifymprime.strmsg, input, input_len);
		hci->verifymprime.str_len = input_len;
	}

	ret = hdcp_tee_comm(node, hci);

	return ret;
}

MODULE_LICENSE("GPL");

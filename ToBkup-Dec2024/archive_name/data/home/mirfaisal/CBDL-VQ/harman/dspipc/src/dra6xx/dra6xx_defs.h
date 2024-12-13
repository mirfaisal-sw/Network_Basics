/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file dra6xx_defs.h
 *
 * This is the header file containing the bit descriptions of the
 * dra6xx Jacinto 5 internal.
 *
 * @author Andreas Abel
 * @date   25.10.2010
 *
 * Copyright (c) 2010 Harman/Becker Automotive Systems GmbH
 */

#ifndef _DRA6XX_DEFS_H_INCLUDED_
#define _DRA6XX_DEFS_H_INCLUDED_

/**
 * Specify the internal offset of the SDRAM starting address
 */
#define DRA44XA_DSP_MEMMAP_REGISTER_OFFSET 0x01800000UL

enum {
        DM642_VENDORID = 0x104C,
        DM642_DEVICEID = 0x9065,
        DM642_SUBSYSTEMID_EVAL_V2 = 0x0642,
        DM642_SUBSYSTEMVENDORID_EVAL_V2 = 0x1652,
        DM642_SUBSYSTEMID_OSGI_B2 = 0x0000,
        DM642_SUBSYSTEMVENDORID_OSGI_B2 = 0x0000
};

enum {
        DRA44XA_PCI_HSR = (0x01C1FFF0UL - DRA44XA_DSP_MEMMAP_REGISTER_OFFSET),
        DRA44XA_PCI_HDCR = (0x01C1FFF4UL - DRA44XA_DSP_MEMMAP_REGISTER_OFFSET),
        DRA44XA_PCI_DSPP = (0x01C1FFF8UL - DRA44XA_DSP_MEMMAP_REGISTER_OFFSET),
        DRA44XA_PCI_RESERVED = (0x01C1FFFCUL - DRA44XA_DSP_MEMMAP_REGISTER_OFFSET),
        DRA44XA_MEM_RSTSRC = (0x01C00000UL) /*   01C0 0000 RSTSRC DSP Reset source/status register */

};

/* PCI Host Ststus Register (HSR) */
enum {
        HSR_INTSRC = 0x00000001UL,
        HSR_INTAVAL = 0x00000002UL,
        HSR_INTAM = 0x00000004UL,
        HSR_CFGERR = 0x00000008UL,
        HSR_EEREAD = 0x00000010UL
};

/* PCI Host-to-DSP Control Register */
enum {
        HDCR_WARMRESET = 0x00000001UL,
        HDCR_DSPINT = 0x00000002UL,
        HDCR_PCIBOOT = 0x00000004UL
};

/* PCI DSP Page Register (DSPP) */
enum {
        DSPP_PAGE_MASK = 0x000003FFUL,
        DSPP_PAGE_SHIFT = 0,
        DSPP_MAP = 0x00000400UL
};

/* Memory map Register (RSTSRC) */
enum {
        RSTSRC_RST = 0x00000001UL,
        RSTSRC_PRST = 0x00000002UL,
        RSTSRC_WARMRST = 0x00000004UL,
        RSTSRC_INTREQ = 0x00000008UL,
        RSTSRC_INTRST = 0x00000010UL,
        RSTSRC_CFGDONE = 0x00000020UL,
        RSTSRC_CFGERR = 0x00000040UL
};

/**
 * Register access mode settings
 */
enum {
        DRA44XA_HSR_REG_R = HSR_INTSRC | /* read capable bits of the hsr register */
            HSR_INTAVAL | HSR_INTAM | HSR_CFGERR | HSR_EEREAD,
        DRA44XA_HSR_REG_W = HSR_INTAM,        /* write capable bits of the hsr register */
        DRA44XA_HSR_REG_WOC = HSR_INTSRC,     /* write one clear capable bits of the hsr register */
        DRA44XA_HDCR_REG_R = HDCR_PCIBOOT,    /* read capable bits of the hdcr register */
        DRA44XA_HDCR_REG_W = HDCR_WARMRESET | /* write capable bits of the hdcr register */
            HDCR_DSPINT,
        DRA44XA_HDCR_REG_WOC = 0x00000000U,   /* write one clear capable bits of the hdcr register */
        DRA44XA_DSPP_REG_R = DSPP_PAGE_MASK | /* read capable bits of the dspp register */
            DSPP_MAP,
        DRA44XA_DSPP_REG_W = DSPP_PAGE_MASK, /* write capable bits of the dspp register */
        DRA44XA_DSPP_REG_WOC = 0x00000000U   /* write one clear capable bits of the dspp register */
};

#endif /*_DRA6XX_DEFS_H_INCLUDED_ */

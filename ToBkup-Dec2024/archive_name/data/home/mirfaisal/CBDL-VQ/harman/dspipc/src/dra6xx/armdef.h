/*****************************************************************************
* Project         Harman Car Multimedia System 
* (c) copyright   2009
* Company         Harman/Becker Automotive Systems GmbH 
*                 All rights reserved 
* Secrecy Level   STRICTLY CONFIDENTIAL 
*****************************************************************************/
/**
 * @file armdef.h
 * 
 * This header file containing the harman/becker ARM definitions
 * for the Jacinto Chip
 * 
 * @author Andreas Abel
 * @date   30.09.2007
 * 
 * Copyright (c) 2007 Harman/Becker Automotive Systems GmbH
 */

#ifndef _ARMDEF_H_INCLUDED_
#define _ARMDEF_H_INCLUDED_

#include <linux/types.h>

/**
 *  Memory regions defnition
 */
enum {
        DSP_L2_RAM = 0x40800000U,      /* C674x� L2 RAM   C674x� L2 RAM */
        DSP_L2_RAM_SIZE = 0x00040000U, /* 256kB */

        DSP_L1_RAM = 0x40800000U,      /* C674x L1P Cache/RAM  */
        DSP_L1_RAM_SIZE = 0x00008000U, /* 32kB */

        DSP_L1D_RAM = 0x40F00000U,      /* C674x L1D Cache/RAM */
        DSP_L1D_RAM_SIZE = 0x00008000U, /* 32kB */

        PSC_PTCMD = 0x00000120,  /* PSC Power Transition Command Register Base Address */
        PSC_PTSTAT = 0x00000128, /* PSC Power Transition State Register Base Address */

        PDCTL0 = 0x00000300,  /* Power Domain Control 0 Register */
        PDSTAT0 = 0x00000200, /* Power Domain Status 0 Register */

        DCM_ARM_IRQ1_CLEAR = 0x0000180CU, /*  0x01CA180CU */
        DCM_ARM_FIQ1_CLEAR = 0x00001804U, /*  0x01CA1804U */

        DCM_ARM_EINT0 = 0x00001818U,  /*  Interrupt Enable 0 register */
        DCM_ARM_EINT1 = 0x0000181CU,  /*  Interrupt Enable 1 register */
        DCM_ARM_INTCTL = 0x00001820U, /*  Interrupt Control register */
        DCM_ARM_EABASE = 0x00001824U, /*  Interrupt EABASE register */

        EMIFA_CS2 = 0x20000000U, /*23FFFFFF 64M EMIFA DATA EMIFA DATA EMIFA DATA EMIFA DATA CS2 EMIFA DATA  CS2 CS2 CS2 CS2 */
        EMIFA_CS3 = 0x24000000U, /*27FFFFFF 64M EMIFA DATA EMIFA DATA EMIFA DATA EMIFA DATA CS3 EMIFA DATA CS3 CS3 CS3 CS3 */
        EMIFA_CS4 = 0x28000000U, /*2BFFFFFF 64M EMIFA DATA EMIFA DATA EMIFA DATA EMIFA DATA CS4 EMIFA DATA CS4 CS4 CS4 CS4 */
        EMIFA_CS5 = 0x2C000000U, /*2FFFFFFF 64M EMIFA DATA EMIFA DATA EMIFA DATA EMIFA DATA CS5 EMIFA DATA CS5 CS5 CS5 CS5 */

        EMIFA_SDRAM = 0x40000000U, /*5FFFFFFF 512M EMIFA DATA EMIFA DATA EMIFA DATA EMIFA DATA CS(1:0) EMIFA DATA CS(1:0) SDRAM CS(1:0) CS(1:0) SDRAM CS(1:0) SDRAM SDRAM SDRAM */
        EMIFB_SDRAM = 0x60000000U, /*7FFFFFFF 512M EMIFB DATA EMIFB DATA EMIFB DATA EMIFB DATA CS(1:0) EMIFB DATA CS(1:0) SDRAM CS(1:0) CS(1:0) SDRAM CS(1:0) SDRAM */

        DSP_MEMORY_SIZE = 0x01000000U, /* 16MB Memory for DSP */

        DSP_MEMORY_SIZE_xxM = 0x02000000U, /* 32MB Memory for DSP   */

        DSP_MEMORY_SIZE_8M = 0x01000000U,   /* 8MB Memory for DSP */
        DSP_MEMORY_SIZE_MASK = 0x00FFFFFFU, /* MASK for 16MB Memory for DSP */

        DUMMYBL_MEMORY_START = 0x65e7f000, /* DSP Start Address for Dummy Bootloader  0x65e7f000   0x11800000U  0x45e7f000U */

        DSP_INTERNAL_MEMORY_START = 0x40800000U, /* Internal DSP Memory Start Address */
        DSP_INTERNAL_MEMORY_SIZE = 0x00040000U,  /* Internal DSP Memory Size */

        DEV_CONF_MODULE = 0x480C8000U,          /*mailbox base address */
        DEV_CONF_MODULE_MEM_SIZE = 0x00001000U, /*4KB */
        DSP_IDLE_CFG_REG = 0x0014061CU,         /* MB to clude the Mailbox Register   Memory for Device Configuaration Module                   */

        MOD_STAT_CRTL_BASE = 0x48180000U,     /*  Power, Reset and Clock Management (PRCM) Module  PRCM Peripheral Registers */
        MOD_STAT_CRTL_MEM_SIZE = 0x00004000U, /* Power, Reset and Clock Management  Size (16k) */

        PM_DSP_PWRSTCTRL = 0x00000A00U, /* RW 32 0x0003 0000 0x00 0x0A00 */
        PM_DSP_PWRSTST = 0x00000A04U,   /* RO 32 0x0000 0017 0x04 0x0A04 */
        RM_DSP_RSTCTRL = 0x00000A10U,   /* RW 32 0x0000 0003 0x10 0x0A10 */
        RM_DSP_RSTST = 0x00000A14U,     /* RW 32 0x0000 0000 0x14 0x0A14 */

        CM_DSP_CLKSTCTRL = 0x00000400U, /* RW 32 0x0000 0001 0x00 0x0400 */
        CM_DSP_CLKCTRL = 0x00000420U,   /* RW 32 0x0007 0000 0x20 0x0420 */

        CM_MMU_CLKSTCTRL = 0x0000140C,
        CM_ALWON_MMUDATA_CLKCTRL = 0x0000159C,

        CM_MMUCFG_CLKSTCTRL = 0x00001410,
        CM_ALWON_MMUCFG_CLKCTRL = 0x000015A8,

        /*McASP CLOCK */
        CM_ALWON_MCASP0_CLKCTRL = 0x00001540,
        CM_ALWON_MCASP1_CLKCTRL = 0x00001544,
        CM_ALWON_MCASP2_CLKCTRL = 0x00001548,
        CM_ALWON_MCASP345_CLKCTRL = 0x0000156C,

};

#define CM_ALWON_MAILBOX_CLKC (0x00001594U) /* CM_ALWON_MAILBOX_CLKC */
#define CM_CLKOUT_CTRL (0x00000100U)        /* CM_CLKOUT_CTRL */

#define DCM_DSPBOOTADDR (0x00140048U) /* CM_ALWON_MAILBOX_CLKC */

#define MOD_DEVCONF_REG 0  /* Selector for DEV_CONF_MODULE */
#define MOD_STATCTRL_REG 1 /* MOD_STAT_CRTL_BASE   (PRCM) Module */

/**
 *  Device Configuration Registers Field Descriptions
 */

/**
 *  Mailbox Register
 */
enum {
        MAILBOX_REVISION = 0x00000000U,  /* Mailbox Revision */
        MAILBOX_SYSCONFIG = 0x00000010U, /* Mailbox System Configuration */

        MAILBOX_MESSAGE_m = 0x00000040U,    /* Mailbox Message Register      0x0000 0040 + (0x4 * m) */
        MAILBOX_FIFOSTATUS_m = 0x00000080U, /* Mailbox FIFO Status Register     0x0000 0080 + (0x4 * m) */
        MAILBOX_MSGSTATUS_m = 0x000000C0U,  /* Mailbox Message Status Register    0x0000 00C0 + (0x4 * m) */

        MAILBOX_IRQSTATUS_RAW_u = 0x00000100U, /* Mailbox IRQ RAW Status Register    0x0000 0100 + (0x10 * u) */
        MAILBOX_IRQSTATUS_CLR_u = 0x00000104U, /* Mailbox IRQ Clear Status Register    0x0000 0104 + (0x10 * u) */
        MAILBOX_IRQENABLE_SET_u = 0x00000108U, /* Mailbox IRQ Enable Set Register      0x0000 0108 + (0x10 * u) */
        MAILBOX_IRQENABLE_CLR_u = 0x0000010CU  /* Mailbox IRQ Enable Clear Register    0x0000 010C + (0x10 *u) */
};

#define MAILBOX_MESSAGE(m) (0x00000040U + (0x4 * m))        /* Mailbox Message Register           0x0000 0040 + (0x4 * m) */
#define MAILBOX_FIFOSTATUS(m) (0x00000080U + (0x4 * m))     /* Mailbox FIFO Status Register       0x0000 0080 + (0x4 * m) */
#define MAILBOX_MSGSTATUS(m) (0x000000C0U + (0x4 * m))      /* Mailbox Message Status Register    0x0000 00C0 + (0x4 * m) */
#define MAILBOX_IRQSTATUS_RAW(u) (0x00000100U + (0x10 * u)) /* Mailbox IRQ RAW Status Register    0x0000 0100 + (0x10 * u) */

#define MAILBOX_IRQSTATUS_CLR_REG(u) (0x00000104U + (0x10 * u)) /* MAILBOX_IRQENABLE_SET_u (2) RW Mailbox IRQ Status Clear Register 0x0000 0104 + (0x10 * u) */
#define MAILBOX_IRQENABLE_SET_REG(u) (0x00000108U + (0x10 * u)) /* MAILBOX_IRQENABLE_SET_u (2) RW Mailbox IRQ Enable Set Register 0x0000 0108 + (0x10 * u) */
#define MAILBOX_IRQENABLE_CLR_REG(u) (0x0000010CU + (0x10 * u)) /* MAILBOX_IRQENABLE_SET_u (2) RW Mailbox IRQ Clear Register 0x0000 010C + (0x10 * u) */

#define MAILBOX_IRQENABLE_NEWMSG_SET(a) (0x00000001U << a) /* enable NEWMSG INT of mailbox a */

/**
 *  Register defines for the ARMAUX_CTL0 Register
 */
enum {
        ARMAUX_CTL0_CLKOBS_SEL_12 = 0x00002000U, /* DSP to ARM INT1 Status CLEAR command bit. */
        ARMAUX_CTL0_CLKOBS_SEL_24 = 0x00002000U, /* DSP to ARM INT1 Status CLEAR command bit. */
        ARMAUX_CTL0_ARM_UNLOCK = 0x00090000U,    /* These bits are used to lock the memory-mapped registers of PLL0, PLL1, PSC controllers, */
                                                 /* MPROTA and MPROTB. Once locked, writes to these registers have no effect */
        ARMAUX_CTL0_ARM_LOCK = 0x000F0000U,      

        ARMAUX_CTL0_ARM_LOCK_MASK = 0x000F0000U, /* Internal bus error enable. This bit enables the interrupt generated to the ARM interrupt handler */
        ARMAUX_CTL0_VERROR_EN = 0x00000080U,     /* Internal bus error enable. This bit enables the interrupt generated to the ARM interrupt handler */
                                                 /* based on internal bus error trap logic. */
        ARMAUX_CTL0_EMIFA_CLKDIS = 0x00000040U,  /* EMIFA clock disable / 1:EMIFA_CLK is forced to high impedance state */
        ARMAUX_CTL0_INIT_DONE = 0x00000020U      /* INIT_DONE bit. Indicates that ARM is done with EMIF configuration and allows full control */
                                                 /* of EMIF pins to the peripheral/controller. */
};

/**
 *  Register defines for the ARM_INTCMD Register
 */
enum {
        ARM_INTCMD_ARM1STAT_CLR = 0x00002000U, /* DSP to ARM INT1 Status CLEAR command bit. */
        ARM_INTCMD_ARM0STAT_CLR = 0x00001000U, /* DSP to ARM INT0 Status CLEAR command bit. */

        ARM_INTCMD_INTDSP3_SET = 0x00000080U,    /* ARM to DSP INT3 Set bit. */
        ARM_INTCMD_INTDSP2_SET = 0x00000040U,    /* ARM to DSP INT2 Set bit. */
        ARM_INTCMD_INTDSP1_SET = 0x00000020U,    /* ARM to DSP INT1 Set bit. */
        ARM_INTCMD_INTDSP0_SET = 0x00000010U,    /* ARM to DSP INT0 Set bit. */
        ARM_INTCMD_INTDSP_NMI_SET = 0x00000001U, /* ARM to DSP NMI Set bit */
};

/**
 *  Register defines for the ARM_INTSTAT Register
 */
enum {
        ARM_INTSTAT_INTARM1_STAT = 0x00002000U,     /* DSP to ARM INT1 Status. */
        ARM_INTSTAT_INTARM0_STAT = 0x00001000U,     /* DSP to ARM INT0 Status. */
        ARM_INTSTAT_INTARM_STAT_MASK = 0x00003000U, /* MASK for DSP to ARM INT0 Status Flags */

        ARM_INTSTAT_INTDSP3_STAT = 0x00000080U,     /* ARM to DSP INT3 Status */
        ARM_INTSTAT_INTDSP2_STAT = 0x00000040U,     /* ARM to DSP INT2 Status */
        ARM_INTSTAT_INTDSP1_STAT = 0x00000020U,     /* ARM to DSP INT1 Status */
        ARM_INTSTAT_INTDSP0_STAT = 0x00000010U,     /* ARM to DSP INT0 Status */
        ARM_INTSTAT_INTDSP_STAT_MASK = 0x00000070U, /* DSP INT Status MASK */
        ARM_INTSTAT_INTDSP_NMI_STAT = 0x00000001U,  /* ARM to DSP NMI Status */
};

/**
 *  Register defines for the DSP_INTCMD Register
 */
enum {
        DSP_INTCMD_INTARM1_SET = 0x00002000U,    /* DSP to ARM INT1 Command bit */
        DSP_INTCMD_INTARM0_SET = 0x00001000U,    /* DSP to ARM INT0 Command bit */
        DSP_INTCMD_INTDSP3_CLR = 0x00000080U,    /* ARM to DSP INT3 Status CLEAR command bit */
        DSP_INTCMD_INTDSP2_CLR = 0x00000040U,    /* ARM to DSP INT2 Status CLEAR command bit */
        DSP_INTCMD_INTDSP1_CLR = 0x00000020U,    /* ARM to DSP INT1 Status CLEAR command bit */
        DSP_INTCMD_INTDSP0_CLR = 0x00000010U,    /* ARM to DSP INT0 Status CLEAR command bit */
        DSP_INTCMD_INTDSP_NMI_CLR = 0x00000001U, /* ARM to DSP INT2 Status CLEAR command bit. */
};

/**
 *  Register defines for the DSP_INTSTAT Register
 */
enum {
        DSP_INTSTAT_INTARM1_STAT = 0x00002000U,     /* DSP to ARM INT1 Status */
        DSP_INTSTAT_INTARM0_STAT = 0x00001000U,     /* DSP to ARM INT0 Status */
        DSP_INTSTAT_INTARM_STAT_MASK = 0x00003000U, /* MASK for DSP to ARM INT Status Flags */
        DSP_INTSTAT_INTDSP3_STAT = 0x00000080U,     /* ARM to DSP INT3 Status */
        DSP_INTSTAT_INTDSP2_STAT = 0x00000040U,     /* ARM to DSP INT2 Status */
        DSP_INTSTAT_INTDSP1_STAT = 0x00000020U,     /* ARM to DSP INT1 Status */
        DSP_INTSTAT_INTDSP0_STAT = 0x00000010U,     /* ARM to DSP INT0 Status */
        DSP_INTSTAT_INTDSP_NMI_STAT = 0x00000001U,  /* ARM to DSP NMI status */
};

enum {
        MDSTAT_MOD_0_ADR = 0x00000800,  /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_1_ADR = 0x00000804,  /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_2_ADR = 0x00000808,  /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_3_ADR = 0x0000080C,  /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_4_ADR = 0x00000810,  /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_5_ADR = 0x00000814,  /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_6_ADR = 0x00000818,  /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_7_ADR = 0x0000081C,  /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_8_ADR = 0x00000820,  /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_9_ADR = 0x00000824,  /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_10_ADR = 0x00000828, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_11_ADR = 0x0000082C, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_12_ADR = 0x00000830, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_13_ADR = 0x00000834, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_14_ADR = 0x00000838, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_15_ADR = 0x0000083C, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_16_ADR = 0x00000840, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_17_ADR = 0x00000844, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_18_ADR = 0x00000848, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_19_ADR = 0x0000084C, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_20_ADR = 0x00000850, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_21_ADR = 0x00000854, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_22_ADR = 0x00000858, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_23_ADR = 0x0000085C, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_24_ADR = 0x00000860, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_25_ADR = 0x00000864, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_26_ADR = 0x00000868, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_27_ADR = 0x0000086C, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_28_ADR = 0x00000870, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_29_ADR = 0x00000874, /* Modul Ctrl Reg Addr */
                                        /* MDSTAT_MOD_30_ADR       = 0x00000878,  Modul Ctrl Reg Addr */
                                        /* MDSTAT_MOD_31_ADR       = 0x0000087C,  Modul Ctrl Reg Addr */
                                        /* MDSTAT_MOD_32_ADR       = 0x00000880,  Modul Ctrl Reg Addr */
                                        /* MDSTAT_MOD_33_ADR       = 0x00000884,  Modul Ctrl Reg Addr */
        MDSTAT_MOD_34_ADR = 0x00000888, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_35_ADR = 0x0000088C, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_36_ADR = 0x00000890, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_37_ADR = 0x00000894, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_38_ADR = 0x00000898, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_39_ADR = 0x0000089C, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_40_ADR = 0x000008A0, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_41_ADR = 0x000008A4, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_42_ADR = 0x000008A8, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_43_ADR = 0x000008AC, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_44_ADR = 0x000008B0, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_45_ADR = 0x000008B4, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_46_ADR = 0x000008B8, /* Modul Ctrl Reg Addr */
        MDSTAT_MOD_47_ADR = 0x000008BC, /* Modul Ctrl Reg Addr */
                                        /* MDSTAT_MOD_48_ADR       = 0x00008AC8,  Modul Ctrl Reg Addr */
        MDSTAT_MOD_49_ADR = 0x000008C4, /* Modul Ctrl Reg Addr       */
};

enum {
        MDCTL_MOD_0_ADR = 0x00000A00,  /* Modul Ctrl Reg Addr */
        MDCTL_MOD_1_ADR = 0x00000A04,  /* Modul Ctrl Reg Addr DSP */
        MDCTL_MOD_2_ADR = 0x00000A08,  /* Modul Ctrl Reg Addr */
        MDCTL_MOD_3_ADR = 0x00000A0C,  /* Modul Ctrl Reg Addr */
        MDCTL_MOD_4_ADR = 0x00000A10,  /* Modul Ctrl Reg Addr */
        MDCTL_MOD_5_ADR = 0x00000A14,  /* Modul Ctrl Reg Addr */
        MDCTL_MOD_6_ADR = 0x00000A18,  /* Modul Ctrl Reg Addr */
        MDCTL_MOD_7_ADR = 0x00000A1C,  /* Modul Ctrl Reg Addr */
        MDCTL_MOD_8_ADR = 0x00000A20,  /* Modul Ctrl Reg Addr */
        MDCTL_MOD_9_ADR = 0x00000A24,  /* Modul Ctrl Reg Addr */
        MDCTL_MOD_10_ADR = 0x00000A28, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_11_ADR = 0x00000A2C, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_12_ADR = 0x00000A30, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_13_ADR = 0x00000A34, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_14_ADR = 0x00000A38, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_15_ADR = 0x00000A3C, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_16_ADR = 0x00000A40, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_17_ADR = 0x00000A44, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_18_ADR = 0x00000A48, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_19_ADR = 0x00000A4C, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_20_ADR = 0x00000A50, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_21_ADR = 0x00000A54, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_22_ADR = 0x00000A58, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_23_ADR = 0x00000A5C, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_24_ADR = 0x00000A60, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_25_ADR = 0x00000A64, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_26_ADR = 0x00000A68, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_27_ADR = 0x00000A6C, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_28_ADR = 0x00000A70, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_29_ADR = 0x00000A74, /* Modul Ctrl Reg Addr */
                                       /* MDCTL_MOD_30_ADR       = 0x00000A78,  Modul Ctrl Reg Addr */
                                       /* MDCTL_MOD_31_ADR       = 0x00000A7C,  Modul Ctrl Reg Addr */
                                       /* MDCTL_MOD_32_ADR       = 0x00000A80,  Modul Ctrl Reg Addr */
                                       /* MDCTL_MOD_33_ADR       = 0x00000A84,  Modul Ctrl Reg Addr */
        MDCTL_MOD_34_ADR = 0x00000A88, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_35_ADR = 0x00000A8C, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_36_ADR = 0x00000A90, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_37_ADR = 0x00000A94, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_38_ADR = 0x00000A98, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_39_ADR = 0x00000A9C, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_40_ADR = 0x00000AA0, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_41_ADR = 0x00000AA4, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_42_ADR = 0x00000AA8, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_43_ADR = 0x00000AAC, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_44_ADR = 0x00000AB0, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_45_ADR = 0x00000AB4, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_46_ADR = 0x00000AB8, /* Modul Ctrl Reg Addr */
        MDCTL_MOD_47_ADR = 0x00000ABC, /* Modul Ctrl Reg Addr */
                                       /* MDCTL_MOD_48_ADR       = 0x00000AC8,  Modul Ctrl Reg Addr */
        MDCTL_MOD_49_ADR = 0x00000AC4, /* Modul Ctrl Reg Addr       */
};

/**
 *  Module Status Register 0-49 (MDSTATn) Field Descriptions
 */
enum {
        MDSTAT_EMURST = 0x00002000U,         /* 'Emulation Alters Module Reset' Interrupt active */
        MDSTAT_MCKOUT = 0x00001000U,         /* Module clock output status. Shows status of module clock on / off. */
        MDSTAT_MRSTDONE = 0x00000800U,       /* Module reset done. Software is responsible for checking that mode reset is done */
        MDSTAT_MRST = 0x00000400U,           /* Module reset status. Reflects actual state of module reset. */
        MDSTAT_LRSTDONE = 0x00000200U,       /* Local reset done. Software is responsible for checking if local reset is done before accessing */
        MDSTAT_RESET_DONE_MSK = 0x00000A00U, /* to check Local reset and Module reset done */
        MDSTAT_LRST = 0x00000100U,           /* Module local reset status (this bit applies to the ARM and DSP module only). */
        MDSTAT_STATE_MSK = 0x0000001FU,      /* Module state status: indicates current module status. */
        MDSTAT_STATE_SWRSTDIS = 0x00000000U, /* Module state status: indicates current module status. */
        MDSTAT_STATE_SYNCRST = 0x00000001U,  /* Module state status: indicates current module status. */
        MDSTAT_STATE_DISABLE = 0x00000002U,  /* Module state status: indicates current module status. */
        MDSTAT_STATE_ENABLE = 0x00000003U,   /* Module state status: indicates current module status. */
        /*   MDSTAT_STATE_TRANS     = 0x00000001U,  Module state status: indicates current module status. */
};

/**
 *  Module Control Register 0-49 (MDCTLn) Field Descriptions
 */
enum {
        MDCTL_EMURSTIE = 0x00002000U,      /* Interrupt enable for emulation alters reset. */
        MDCTL_MRST = 0x00000400U,          /* Module reset is de-asserted. */
        MDCTL_LRST_DONE = 0x00000200U,     /* Local reset is done. */
        MDCTL_LRST = 0x00000100U,          /* Module local reset control (This bit applies to the DSP module only.) */
        MDCTL_NEXT = 0x00000080U,          /* Module next state */
        MDCTL_NEXT_MSK = 0x0000003FU,      /* Module state status: indicates current module status. */
        MDCTL_NEXT_SWRSTDIS = 0x00000000U, /* Module state status: SwRstDisable state */
        MDCTL_NEXT_SYNCRST = 0x00000001U,  /* Module state status: SyncReset state */
        MDCTL_NEXT_DISABLE = 0x00000002U,  /* Module state status: Disable state */
        MDCTL_NEXT_ENABLE = 0x00000003U,   /* Module state status: Enable state */
        MDCTL_NEXT_MSK_LOW = 0xFFFFFF00U,  /* Mask for lower bits (must be Zero) */
        MDCTL_MSK = 0x00000303U,           /* Mask for lower bits (must be Zero) */
};

#define PSC_MODULE_ARM 0
#define PSC_MODULE_DSP 1
#define PSC_MODULE_EMIF_A 13
#define PSC_MODULE_EMIF_B 14
#define PSC_MODULE_I2C0 22
#define PSC_MODULE_I2C1 23

#define PSC_MODULE_I2C2 24
#define PSC_MODULE_I2C3 25
#define PSC_MODULE_SPI1 35
#define PSC_MODULE_SPI2 36
#define PSC_MODULE_SPI3 37

/**
 *  Register defines for the EMIF FPGA cell 
 */
enum {
        EMIF_WB_VERSION_REG = 0x0000U,       /* WB Version Register, provides access to HB standard Version sub-module (R/W) */
        EMIF_WB_PARA_REG = 0x0004U,          /* WB parameter register (vhdl configuration settings of the module) */
        EMIF_WB_CONFIG_REG = 0x0008U,        /* WB Configuration Register, module configuration bits (R/W) */
        EMIF_WB_IOC_CTRL_STAT_REG = 0x0010U, /* WB IO-Controller control/status register (R/C) */
        EMIF_WB_IOC_INT_STAT_REG = 0x0014U,  /* WB IO-Controller interrupt status register (R/C) */
        EMIF_WB_IOC_INT_MASK_REG = 0x001CU,  /* WB IO-Controller interrupt mask register (R/W) */
        EMIF_WB_CTRL_STAT_REG = 0x0020U,     /* WB Control/Status Register, module operation status flags (R/C) */
        EMIF_WB_INT_STAT_REG = 0x0024U,      /* WB Interrupt Status Register, module interrupt request flags (R/C)   */
        EMIF_WB_INT_ENA_REG = 0x0028U,       /* WB Interrupts Enable Register, module interrupt enable bits (R/W) */
        EMIF_WB_INT_MASK_REG = 0x002CU,      /* WB Interrupts Mask Register, module interrupt masking bits (R/W) */
        EMIF_WB_MEDIA_MBX_REG = 0x0030U,     /* WB Media Mail Box Register, transmit and receive Media Mail Box registers (R/W) */
        EMIF_WB_CTRL_MBX_REG = 0x0034U,      /* WB Control Mail Box Register, transmit and receive Control Mail Box registers (R/W) */
        EMIF_WB_IOC_MBX_REG = 0x0038U,       /* WB IO-Controller Mail Box Register, transmit and receive IOC Mail Box registers (R/W) */
        EMIF_WB_DATA_BUFFER_START = 0x8000U, /* WB Data Buffer, provides access to the internal dual-port RAM (R/W) */
        EMIF_WB_DATA_BUFFER_END = 0xFFFFU    /* WB Data Buffer end, provides access to the internal dual-port RAM (R/W) */
};

/**
 * Definitions for the FPGA version register
 */
enum {
        EMIF_VERSION_REG_MODID = 0x00, /* Version select for getting the module id */
        EMIF_VERSION_REG_RES1 = 0x01,  /* Version select for reserved1 info */
        EMIF_VERSION_REG_REV = 0x02,   /* Version select for revision info */
        EMIF_VERSION_REG_RES2 = 0x03,  /* Version select for reserved2 info */
        EMIF_VERSION_REG_YEAR = 0x04,  /* Version select for year info */
        EMIF_VERSION_REG_WEEK = 0x05,  /* Version select for week info */
        EMIF_VERSION_REG_DDVER = 0x06, /* Version select for day/version info */
        EMIF_VERSION_REG_RES3 = 0x07   /* Version select for reserved3 info */
};

/**
 * Definitions for the FPGA config register
 */
enum {
        EMIF_CONFIG_REG_INT_ENA = 0x00000001U,   /* Global interrupt enable for the module */
        EMIF_CONFIG_REG_DSP_RUN = 0x00000002U,   /* DSP is running (=not reset) */
        EMIF_CONFIG_REG_WB_2BANKS = 0x00000010U, /* Splitt DPRAM in two banks for receive/transmit */
        EMIF_CONFIG_REG_RESERVED = 0xFFFFFFECU   /* Reserved bits */
};

/**
 * Definitions for the FPGA parameters register
 */
enum {
        EMIF_PARA_REG_INT_LEVEL_MASK = 0x000000FFU,     /* Interrupt vector used by the module (Read only) */
        EMIF_PARA_REG_INT_LEVEL_SHIFT = 0,              /* Interrupt vector bit position */
        EMIF_PARA_REG_RESERVED = 0x000FFF00U,           /* Reserved bits inside parameter register (Read only) */
        EMIF_PARA_REG_IOC_MBX_SIZE_MASK = 0x00F00000U,  /* Size of the ioc mailbox (Read only) */
        EMIF_PARA_REG_IOC_MBX_SIZE_SHIFT = 0,           /* ioc mailbox size bit position */
        EMIF_PARA_REG_IOC_MBX_SIZE_4BYTE = 0x00400000U, /* Size of the ioc mailbox is 4 byte (Read only) */
        EMIF_PARA_REG_BUS_TYPE_MASK = 0x0F000000U,      /* Bits that defines the emi bus type (Read only) */
        EMIF_PARA_REG_BUS_TYPE_SHIFT = 24,              /* Bus size bit position */
        EMIF_PARA_REG_BUS_TYPE_DRA300 = 0x00000000U,    /* Bus type is TI DRA300/DRI350 (Read only) */
        EMIF_PARA_REG_BUS_TYPE_ST5700 = 0x01000000U,    /* Bus type is ST 5700 (Read only) */
        EMIF_PARA_REG_DPRAM_SIZE_MASK = 0xF0000000U,    /* Mask for the dual port ram size (Read only) */
        EMIF_PARA_REG_DPRAM_SIZE_SHIFT = 28,            /* DPRAM size bit position */
        EMIF_PARA_REG_DPRAM_SIZE_2KB = 0x00000000U,     /* Dual port ram size is 2kByte (Read only) */
        EMIF_PARA_REG_DPRAM_SIZE_4KB = 0x10000000U,     /* Dual port ram size is 4kByte (Read only) */
        EMIF_PARA_REG_DPRAM_SIZE_8KB = 0x20000000U      /* Dual port ram size is 8kByte (Read only) */
};

/**
 * Definitions for the FPGA control/status register
 */
enum {
        EMIF_CTRL_STAT_REG_MRX_RDY = 0x00000001U, /* Media message arrived */
        EMIF_CTRL_STAT_REG_MTX_RDY = 0x00000002U, /* Media message send ready */
        EMIF_CTRL_STAT_REG_CRX_RDY = 0x00000004U, /* Control message arrived */
        EMIF_CTRL_STAT_REG_RESERVED = 0xFFFFFFF8U /* Reserved bits */
};

/**
 * Definitions for the FPGA interrupt status (request) register
 */
enum {
        EMIF_INT_STAT_REG_MRX_RDY = 0x00000001U, /* Media message arrived */
        EMIF_INT_STAT_REG_MTX_RDY = 0x00000002U, /* Media message send ready */
        EMIF_INT_STAT_REG_CRX_RDY = 0x00000004U, /* Control message arrived */
        EMIF_INT_STAT_REG_RESERVED = 0xFFFFFFF8U /* Reserved bits */
};

/**
 * Definitions for the FPGA interrupt enable register
 */
enum {
        EMIF_INT_ENA_REG_MRX_RDY = 0x00000001U, /* Media message arrived */
        EMIF_INT_ENA_REG_MTX_RDY = 0x00000002U, /* Media message send ready */
        EMIF_INT_ENA_REG_CRX_RDY = 0x00000004U, /* Control message arrived */
        EMIF_INT_ENA_REG_RESERVED = 0xFFFFFFF8U /* Reserved bits */
};

/**
 * Definitions for the FPGA interrupt enable register
 */
enum {
        EMIF_INT_MASK_REG_MRX_RDY = 0x00000001U, /* Media message arrived */
        EMIF_INT_MASK_REG_MTX_RDY = 0x00000002U, /* Media message send ready */
        EMIF_INT_MASK_REG_CRX_RDY = 0x00000004U, /* Control message arrived */
        EMIF_INT_MASK_REG_RESERVED = 0xFFFFFFF8U /* Reserved bits */
};

/**
 * Definitions for the FPGA media mailbox receive/transmit register
 */
enum {
        EMIF_MBX_REG_LEN_MASK = 0x0000FFFFU,     /* Media mailbox data length mask */
        EMIF_MBX_REG_LEN_SHIFT = 0,              /* Media mailbox data length position */
        EMIF_MBX_REG_HEADER_MASK = 0xFFFF0000U,  /* Media mailbox header mask */
        EMIF_MBX_REG_HEADER_SHIFT = 16,          /* Media mailbox header position */
        EMIF_MBX_REG_CHN_NUM_MASK = 0x3FFF0000U, /* Media mailbox channel number mask */
        EMIF_MBX_REG_CHN_NUM_SHIFT = 16          /* Media mailbox channel number position */
};

/**
 * Definitions for the mailbox header values used to control the bootloader process
 */
enum {
        BOOTLOADER_ENTRYPOINT = 1,    /* mailbox header entry to mark the entry point */
        BOOTLOADER_SECTION_START = 2, /* mailbox header entry to mark the first packet of section data */
        BOOTLOADER_SECTION_DATA = 3,  /* mailbox header entry to mark a subsequent section data packet */
        BOOTLOADER_END = 4            /* mailbox header entry to mark the last section entry */
};

/**
 * Define the value for the acknowledge value to be written to the control
 * mailbox after a successful reception of the application
 */
enum {
        BOOTLOADER_STARTED_ACK = 0xABCDU, /* value to be written from bootloader after start */
        DOWNLOAD_COMPLETE_ACK = 0xD00FU,  /* value to be written from the bootloader after download */
        START_APPLICATION_ACK = 0x55AAU,  /* value to be written from the load boot table after download application */
        APPLICATION_READY_ACK = 0x1234U   /* value to be written from the application after init ready */
};

/**
 * Definitions for the FPGA control mailbox receive/transmit register
 */
enum {
        EMIF_CBX_REG_DATA_MASK = 0x0000FFFFU, /* Control mailbox data mask */
        EMIF_CBX_REG_LEN_SHIFT = 0,           /* Media mailbox data position */
        EMIF_CBX_REG_RESERVED = 0xFFFF0000U,  /* Reserved bits */
};

/**
 * Definitions for the FPGA io-controller mailbox receive/transmit register
 */
enum {
        EMIF_IBX_REG_DATA_MASK = 0xFFFFFFFFU, /* IOC mailbox data mask */
        EMIF_IBX_REG_LEN_SHIFT = 0,           /* IOC mailbox data position */
        EMIF_IBX_REG_RESERVED = 0x00000000U   /* Reserved bits */
};

/**
 * Register access mode settings 
 */
enum {
        EMIF_VERSION_REG_R = 0x000000FFU,                                                                                                                                             /* read capable bits of the version register */
        EMIF_VERSION_REG_W = 0x00000007U,                                                                                                                                             /* write capable bits of the version register */
        EMIF_VERSION_REG_WOC = 0x00000000U,                                                                                                                                           /* write one clear capable bits of the version register */
        EMIF_CONFIG_REG_R = EMIF_CONFIG_REG_INT_ENA | EMIF_CONFIG_REG_DSP_RUN | EMIF_CONFIG_REG_WB_2BANKS,                                                                            /* read capable bits of the config register */
        EMIF_CONFIG_REG_W = EMIF_CONFIG_REG_INT_ENA | EMIF_CONFIG_REG_DSP_RUN | EMIF_CONFIG_REG_WB_2BANKS,                                                                            /* write capable bits of the config register */
        EMIF_CONFIG_REG_WOC = 0x00000000U,                                                                                                                                            /* write one clear capable bits of the config register */
        EMIF_PARA_REG_W = 0x00000000U,                                                                                                                                                /* write capable bits of the parameter register */
        EMIF_PARA_REG_WOC = 0x00000000U,                                                                                                                                              /* write one clear capable bits of the parameter register */
        EMIF_PARA_REG_R = EMIF_PARA_REG_INT_LEVEL_MASK | EMIF_PARA_REG_IOC_MBX_SIZE_MASK | EMIF_PARA_REG_BUS_TYPE_MASK | EMIF_PARA_REG_BUS_TYPE_MASK | EMIF_PARA_REG_DPRAM_SIZE_MASK, /* read capable bits of the parameter register */
        EMIF_CTRL_STAT_REG_R = EMIF_CTRL_STAT_REG_MRX_RDY | EMIF_CTRL_STAT_REG_MTX_RDY | EMIF_CTRL_STAT_REG_CRX_RDY,                                                                  /* read capable bits of the control/status register */
        EMIF_CTRL_STAT_REG_W = 0x00000000U,                                                                                                                                           /* write capable bits of the config register */
        EMIF_CTRL_STAT_REG_WOC = EMIF_CTRL_STAT_REG_MRX_RDY | EMIF_CTRL_STAT_REG_MTX_RDY | EMIF_CTRL_STAT_REG_CRX_RDY,                                                                /* write one clear capable bits of the control/status register */
        EMIF_INT_STAT_REG_R = EMIF_INT_STAT_REG_MRX_RDY | EMIF_INT_STAT_REG_MTX_RDY | EMIF_INT_STAT_REG_CRX_RDY,                                                                      /* read capable bits of the interrupt status register */
        EMIF_INT_STAT_REG_W = 0x00000000U,                                                                                                                                            /* write capable bits of the interrupt status register */
        EMIF_INT_STAT_REG_WOC = EMIF_INT_STAT_REG_MRX_RDY | EMIF_INT_STAT_REG_MTX_RDY | EMIF_INT_STAT_REG_CRX_RDY,                                                                    /* write one clear capable bits of the interrupt status register */
        EMIF_INT_ENA_REG_R = EMIF_INT_ENA_REG_MRX_RDY | EMIF_INT_ENA_REG_MTX_RDY | EMIF_INT_ENA_REG_CRX_RDY,                                                                          /* read capable bits of the interrupt enable register */
        EMIF_INT_ENA_REG_W = EMIF_INT_ENA_REG_MRX_RDY | EMIF_INT_ENA_REG_MTX_RDY | EMIF_INT_ENA_REG_CRX_RDY,                                                                          /* write capable bits of the interrupt enable register */
        EMIF_INT_ENA_REG_WOC = 0x00000000U,                                                                                                                                           /* write one clear capable bits of the interrupt enable register */
        EMIF_INT_MASK_REG_R = EMIF_INT_MASK_REG_MRX_RDY | EMIF_INT_MASK_REG_MTX_RDY | EMIF_INT_MASK_REG_CRX_RDY,                                                                      /* read capable bits of the interrupt mask register */
        EMIF_INT_MASK_REG_W = EMIF_INT_ENA_REG_MRX_RDY | EMIF_INT_ENA_REG_MTX_RDY | EMIF_INT_ENA_REG_CRX_RDY,                                                                         /* write capable bits of the interrupt mask register */
        EMIF_INT_MASK_REG_WOC = 0x00000000U,                                                                                                                                          /* write one clear capable bits of the interrupt mask register */
        EMIF_MBXRX_REG_R = EMIF_MBX_REG_HEADER_MASK | EMIF_MBX_REG_LEN_MASK,                                                                                                          /* read capable bits of the mbx rx register */
        EMIF_MBXRX_REG_W = 0x00000000U,                                                                                                                                               /* write capable bits of the mbx rx register */
        EMIF_MBXRX_REG_WOC = 0x00000000U,                                                                                                                                             /* write one clear capable bits of the mbx rx register */
        EMIF_MBXTX_REG_R = 0x00000000U,                                                                                                                                               /* read capable bits of the mbx tx register */
        EMIF_MBXTX_REG_W = EMIF_MBX_REG_HEADER_MASK | EMIF_MBX_REG_LEN_MASK,                                                                                                          /* write capable bits of the mbx tx register */
        EMIF_MBXTX_REG_WOC = 0x00000000U,                                                                                                                                             /* write one clear capable bits of the mbx tx register */
        EMIF_CBXRX_REG_R = EMIF_CBX_REG_DATA_MASK,                                                                                                                                    /* read capable bits of the cbx rx register */
        EMIF_CBXRX_REG_W = 0x00000000U,                                                                                                                                               /* write capable bits of the cbx rx register */
        EMIF_CBXRX_REG_WOC = 0x00000000U,                                                                                                                                             /* write one clear capable bits of the cbx rx register */
        EMIF_CBXTX_REG_R = 0x00000000U,                                                                                                                                               /* read capable bits of the cbx rx register */
        EMIF_CBXTX_REG_W = EMIF_CBX_REG_DATA_MASK,                                                                                                                                    /* write capable bits of the cbx rx register */
        EMIF_CBXTX_REG_WOC = 0x00000000U                                                                                                                                              /* write one clear capable bits of the cbx rx register */
};

#endif /*_ARMDEF_H_INCLUDED_ */

/**************************************************************
 * Project			Harman Car Multimedia System
 * (c) Copyright	2005
 * Company			Harman/Becker Automotive Systems GmbH
 *					All rights reserved
 **************************************************************/
/**
 * @file			emif.h
 *
 * This file specifies preprocessor defines that define the constants
 * (register offsets, register bit fiels, value and start addresses)
 * to access the EMIF based DSPIPC FPGA cell
 * 
 * @ingroup		dspipc_emif
 * @date       22.06.2006
 * @author		Ulrich Mohr
 */

#ifndef DRA300EMIF_H
#define DRA300EMIF_H

/** 
 * @brief EMIF registers and data are start addresses.
 * 
 * These two values give the base addresses of the EMIF address
 * spaces. There is one address space containing the EMIF registers
 * and a second one, which can be used to access the blockram that 
 * can be accessed from both DSP and host processor. 
 */
/*@{*/
#define EMIREGS_START_ADDRESS 0xa0000000U
#define EMIDATA_START_ADDRESS 0x90000000U
/*@}*/

#define OLD_EMIF_CELL 1
#define NEW_EMIF_CELL 2

#define CELL NEW_EMIF_CELL

/**  
 * @brief EMIF register offsets
 * 
 * The complete address of the register is given by 
 * EMIREGS_START_ADDRESS + <reg_offs>
 */
/*@{*/
#if (CELL == OLD_EMIF_CELL) /* OLD EMIF CELL */

#define EMIF_CONFIG_REG 0x00U
#define EMIF_CONTROL_REG 0x04U
#define EMIF_STATUS_REG 0x04U /* status (R) and control (W) share the same offset */
#define EMIF_INTREQ_REG 0x10U
#define EMIF_INTENABLE_REG 0x14U
#define EMIF_INTMASK_REG 0x18U
#define EMIF_MEDIAMAILBOXLEN_REG 0x20U
#define EMIF_MEDIAMAILBOXHDR_REG 0x24U
#define EMIF_CONTROLMAILBOX_REG 0x28U
#define EMIF_IOCMAILBOXHIGH_REG 0x2CU
#define EMIF_IOCMAILBOXLOW_REG 0x30U

#else

#define EMIF_CONFIG_REG 0x00U
#define EMIF_CONTROL_REG 0x02U
#define EMIF_STATUS_REG 0x02U /* status (R) and control (W) share the same offset */
#define EMIF_INTREQ_REG 0x04U
#define EMIF_INTENABLE_REG 0x06U
#define EMIF_INTMASK_REG 0x08U
#define EMIF_MEDIAMAILBOXLEN_REG 0x10U
#define EMIF_MEDIAMAILBOXHDR_REG 0x12U
#define EMIF_CONTROLMAILBOX_REG 0x14U
#define EMIF_IOCMAILBOXLOW_REG 0x18U
#define EMIF_IOCMAILBOXHIGH_REG 0x1AU

#endif
/*@}*/

/** 
 * @brief EMIF configuration register values
 * 
 * The following set of values define flag values for the 
 * emif configuration register. To get the value of a specific flag, 
 * AND the register value of the confiuration register with the 
 * desired flag field
 */
/*@{*/
#define EMIF_CFG_INT_ENABLE 0x0001
#define EMIF_CFG_2BANK 0x0020
#define EMIF_CFG_SYNC_MODE 0x0080

#define EMIF_CFG_DPRAMSIZE_MSK 0xf000
#define EMIF_CFG_DPRAMSIZE_2KB 0x0000
#define EMIF_CFG_DPRAMSIZE_4KB 0x1000
#define EMIF_CFG_DPRAMSIZE_8KB 0x2000
/*@}*/

/**
 * @brief EMIF control register values
 * 
 * The following set of values define possible flag values for 
 * the EMIF configuration values. 
 * To set the value of the register, AND all flag values you want to set 
 * and write this value to the register. 
 */
/*@{*/
#define EMIF_CTRL_RDCRESET 0x0010
#define EMIF_CTRL_WDCRESET 0x0020

#define EMIF_CTRL_MRX_RDY_CLEAR 0x0001
#define EMIF_CTRL_MTX_RDY_CLEAR 0x0002
#define EMIF_CTRL_CRX_RDY_CLEAR 0x0004
#define EMIF_CTRL_IOC_RDY_CLEAR 0x0008
/*@}*/

/** 
 * @brief EMIF status register values
 * 
 * The following set of values defines possible flag values for the 
 * EMIF status register. To get the value of a specific flag, 
 * AND the the register value of the status register with the desired flag
 * field. 
 */
/*@{*/
#define EMIF_STATUS_MRX_RDY_SET 0x0001
#define EMIF_STATUS_MTX_RDY_SET 0x0002
#define EMIF_STATUS_CRX_RDY_SET 0x0004
#define EMIF_STATUS_IOC_RDY_SET 0x0008
/*@}*/

/**
 * @brief EMIF interrupt request register values
 * 
 * The following set of values define flag values for the emif interrupt 
 * request register. 
 */
/*@{*/
#define EMIF_IRQ_MRX_RDY_SET 0x0001
#define EMIF_IRQ_MTX_RDY_SET 0x0002
#define EMIF_IRQ_CRX_RDY_SET 0x0004
#define EMIF_IRQ_IOC_RDY_SET 0x0008

#define EMIF_IRQ_MRX_RDY_CLEAR 0x0001
#define EMIF_IRQ_MTX_RDY_CLEAR 0x0002
#define EMIF_IRQ_CRX_RDY_CLEAR 0x0004
#define EMIF_IRQ_IOC_RDY_CLEAR 0x0008
/*@}*/

/** 
 * @brief EMIF interrupt enable register values
 * 
 * The following set of values define flag values for the emif interrupt
 * enable register. 
 */
/*@{*/
#define EMIF_IEN_MRX_RDY_ENABLE 0x0001
#define EMIF_IEN_MTX_RDY_ENABLE 0x0002
#define EMIF_IEN_CRX_RDY_ENABLE 0x0004
#define EMIF_IEN_IOC_RDY_ENABLE 0x0008
/*@}*/

/**
 * @brief EMF interrupt mask register values
 * 
 * The following set of values define flag values for the emif interrupt
 * mask register
 */
/*@{*/
#define EMIF_MSK_MRX_RDY_MASKED 0x0001
#define EMIF_MSK_MTX_RDY_MASKED 0x0002
#define EMIF_MSK_CRX_RDY_MASKED 0x0004
#define EMIF_MSK_IOC_RDY_MASKED 0x0008
/*@}*/

#endif /* DRA300EMIF_H */

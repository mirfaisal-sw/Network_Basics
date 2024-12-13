/* ========================================================================
 *
 *               ____    _____   _   _          ___
 *              |  ___| |  ___| | | | |        |   `
 *              | |_    | |_    | |_| |  ____  | [] |
 *              |  _|   |  _|   |  _  | |____| |  _ <
 *              | |___  | |___  | | | |        | |_| |
 *              |_____| |_____| |_| |_|        |____/
 *
 *
 * Project:  ARM BIOS/IPL
 * Module:   coff.h -- Common object file format definitions.
 * Author:   Dr. Bruno Achauer, EEH-B
 *
 * Copyright (c) 2006-2009 Harman/Becker Automotive Systems GmbH
 * All rights reserved.
 *
 * This source code contains confidential information of Harman/Becker
 * Automotive Systems GmbH. (HBAS). Any use, reproduction, modification,
 * disclosure, distribution or transfer of this software, or any software
 * which includes or is based upon any of this code, is only permitted as
 * expressly authorized by a written license agreement from HBAS.
 * ========================================================================
 * $Header: //BiosGroup/trunk/BC_Projects/A024/A024-4010-02GA/DRA6xx-CORTEX_A8/src/std/coff.h#1 $ 
 * $Change : 1967142 $
 * == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == 
 */

#ifndef _COFF_H_
#define _COFF_H_
/*
**  COFF (Common Object File Format), as used by the TI DRI3xx toolchain
**  [these definitions are derived (mostly) from SPRU186N, Appendix A].
*/

#include <linux/types.h>

typedef struct /* COFF file header:      */
{
        uint16_t f_magic;     /* magic number         */
        uint16_t f_nscns;     /* number of sections      */
        uint32_t f_timdat;    /* time & date stamp    */
        uint32_t f_symptr;    /* file pointer to symtab  */
        uint32_t f_nsyms;     /* number of symtab entries   */
        uint16_t f_opthdr;    /* sizeof(optional hdr)    */
        uint16_t f_flags;     /* flags       */
        uint16_t f_target_id; /* (TI COFF specific)      */
} COFF_FileHeader;

#define FILHDRSZ 22 /* *not* sizeof(...); in-file version is packed! */

/* File header flags */
#define F_RELFLG 0x0001 /* relocation info stripped from file */
#define F_EXEC 0x0002   /* there are no unresolved ext. refs */
#define F_LITTLE 0x0100 /* litte endian (16 bits/word, LSB first) */
#define F_BIG 0x0200    /* big endian       */

#define ERROR_COFF_FMT 0x00000004
#define ERROR_COFF_MEM 0x00000008
#define ERROR_MISALIGNED_BUF 0x00000010

typedef struct /* COFF optional file header:  */
{
        uint16_t oh_magic;      /* type of file (0x108)    */
        uint16_t oh_vstamp;     /* version stamp     */
        uint32_t oh_tsize;      /* text size (bytes, padded to word) */
        uint32_t oh_dsize;      /* initialized data "  "   */
        uint32_t oh_bsize;      /* uninitialized data "   "   */
        uint32_t oh_entry;      /* entry point       */
        uint32_t oh_text_start; /* base of text used for this file */
        uint32_t oh_data_start; /* base of data used for this file */
} COFF_AoutHeader;
#define AOUTHDRSZ sizeof(COFF_AoutHeader)

typedef struct /* COFF section header:     */
{
        char s_name[8];      /* section name         */
        uint32_t s_paddr;    /* physical address     */
        uint32_t s_vaddr;    /* virtual address      */
        uint32_t s_size;     /* section size            */
        uint32_t s_scnptr;   /* file ptr to raw data    */
        uint32_t s_relptr;   /* file ptr to relocation  */
        uint32_t s_lnnoptr;  /* reserved [file ptr to line nos] */
        uint32_t s_nreloc;   /* number of relocation entries  */
        uint32_t s_nlnno;    /* reserved [number of line nos] */
        uint32_t s_flags;    /* flags       */
        uint16_t s_reserved; /* reserved                     */
        uint16_t s_page;     /* section page number (LOAD)   */
} COFF_SectionHeader;
#define SCNHDRSZ sizeof(COFF_SectionHeader)

/* Symbol table entry -- not used here. */
#define SYMESZ 18

/*
 * END of COFF definitions
 */

extern uint32_t coff_file_size(void* hdr);
extern uint32_t load_coff_file(uint32_t iaddr, int verbose);
extern int dump_coff_file(uint32_t iaddr, int verbose);
extern int fileScan(unsigned erase);

#endif

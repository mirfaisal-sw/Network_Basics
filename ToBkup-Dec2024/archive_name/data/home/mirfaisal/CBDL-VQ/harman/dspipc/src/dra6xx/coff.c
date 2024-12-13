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
 * Module:   coff.c -- Common object file format support routines.
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
 * $Header: //BiosGroup/trunk/BC_Projects/A024/A024-4010-02GA/DRA6xx-CORTEX_A8/src/std/coff.c#7 $
 * $Change: 2183534 $* 
 == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
 */

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>

#include "../debug.h"
#include "../dev-dspipc.h"
#include "../malloc_trc.h"
#include "../string_def_ext.h"
#include "coff.h"

#define USE_DRA6XX_MEM_ACC

#define USE_HBSLOG

#define SW_FLASH 0

#include "dspipc-dra6xx.h"
/**
 * Define the chipset handle type to the dra6xx handle type
 */
#define dspIpcChipset_t dspIpcDra6xx_t

/**
 *  types for memory access directions
 * @enum MEMACCESSDIR_E
 */
enum MEMACCESSDIR_E {
        MEM_READ, /* set access direction to read */
		MEM_WRITE /* set access direction to write */
};
/**
 *  Create a typedef to use the MEMACCESSDIR_E enumeration
 * @typedef memAccessDir_t
 */
typedef enum MEMACCESSDIR_E memAccessDir_t;

extern int dra6xxAccessMem32(dspIpcChipset_t* pChipset,
    uint32_t addr,
    uint32_t numLongWords,
    memAccessDir_t dir,
    uint32_t* pVal);

extern int dra6xxAccessMem8(dspIpcChipset_t* pChipset,
    uint32_t addr,
    uint32_t numBytes,
    memAccessDir_t dir,
    uint8_t* pVal);

#define coffGetStr(base, off) (((const char*)(base)) + (off))

/**
 * determine size of a COFF file
 *
 * @param   hdr      address of the COFF file
 * @return  file size, or 0 if no COFF file present.
 */
uint32_t
coff_file_size(void* hdr)
{
        COFF_FileHeader fhdr;
        off_t strbase;
        uint32_t strsize;

        memcpy(&fhdr, hdr, FILHDRSZ);

        if (fhdr.f_magic != 0xc2 || fhdr.f_target_id != 0x99) {
                DEBUG_ERROR("** unexpected values in header (not a C6000 COFF file?)\n");
                return -1;
        }

        if (fhdr.f_opthdr && fhdr.f_opthdr != AOUTHDRSZ) {
                DEBUG_ERROR("** unexpected (%d) optional header size\n", fhdr.f_opthdr);
                return -1;
        }

        strbase = fhdr.f_symptr + fhdr.f_nsyms * SYMESZ;
        memcpy(&strsize, hdr + strbase, sizeof(strbase));

        return (strbase + strsize);
}

/**
 * load a COFF file
 *
 * @param   iaddr    address of the COFF file
 * @param   verbose  verbosity flag
 * @return  entry point, or -1 on error.
 */
uint32_t load_coff_file(uint32_t iaddr, int verbose)
{
        COFF_FileHeader fhdr;
        COFF_AoutHeader ohdr;
        COFF_SectionHeader* shdr;
        off_t strbase;
        uint32_t topaddr = iaddr;

        ssize_t nb, nl;
        int i;

        memcpy(&fhdr, (void*)topaddr, FILHDRSZ);
        topaddr += FILHDRSZ;

        if (fhdr.f_magic != 0xc2 || fhdr.f_target_id != 0x99) {
                DEBUG_ERROR("** unexpected values in header (not a C6000 COFF file?)");
                mdelay(2);

                return -1;
        }

        strbase = fhdr.f_symptr + fhdr.f_nsyms * SYMESZ;

        if (fhdr.f_opthdr != AOUTHDRSZ) {
                DEBUG_ERROR("** unexpected (%d) optional header size", fhdr.f_opthdr);
                mdelay(2);

                return -1;
        }

        if (fhdr.f_opthdr) {
                memcpy(&ohdr, (void*)topaddr, AOUTHDRSZ);
                topaddr += AOUTHDRSZ;
        }

        nb = fhdr.f_nscns * SCNHDRSZ;
        shdr = VMALLOC_TRC(nb);
        if (shdr == NULL) {
                DEBUG_ERROR("malloc(%d) failed!", nb);
                mdelay(2);
                return -1;
        }
        memcpy(shdr, (void*)topaddr, nb);

        topaddr += nb;

        for (i = 0, nl = 0; i < fhdr.f_nscns; i += 1) {
                int useit = (shdr[i].s_flags & 0x90) == 0 && shdr[i].s_size != 0 && shdr[i].s_scnptr != 0;

                if (useit && shdr[i].s_size) {
                        uintptr_t dst = shdr[i].s_vaddr;
                        uintptr_t src = shdr[i].s_scnptr + iaddr;
                        uintptr_t len = shdr[i].s_size;

                        if (verbose) {
                                DEBUG_ERROR("-- COPY: %8x -> %8x..%08x", (uint32_t)src, (uint32_t)dst, (uint32_t)(dst + len - 1));
                                mdelay(2);
                        }

                        if (dst >= 0x800000 && dst < 0x1000000) /* in DSP L1,L2 RAM? */
                                dst += 0x40000000;              /* adjust ARM address view */

#ifndef USE_DRA6XX_MEM_ACC
                        memmove((void*)dst, (const void*)src, len);
#else
                        if (dra6xxAccessMem8(pChipset_Coff,
                                dst,
                                (len),
                                MEM_WRITE,
                                (uint8_t*)src)
                            != 0) {
                                DEBUG_ERROR("%s: Write failed", __FUNCTION__);
                        }
#endif

                        if (verbose) {
                                DEBUG_ERROR("%s%x", (nl ? "+" : ""), (uint32_t)len);
                                mdelay(2);
                        }
                        nl += 1;
                }
        }
        VFREE_TRC(shdr);

        if (verbose) {
                DEBUG_ERROR(" -> %x", ohdr.oh_entry);
                mdelay(2);
        }

        return ohdr.oh_entry;
}

/**
 * dump headers of a COFF file
 *
 * @param   iaddr    address of the COFF file
 * @return  error code
 */
int dump_coff_file(uint32_t iaddr, int verbose)
{
        COFF_FileHeader fhdr;
        COFF_AoutHeader ohdr;
        COFF_SectionHeader* shdr;
        off_t strbase;
        uint32_t topaddr = iaddr;

        ssize_t nb;
        int i;

        memcpy(&fhdr, (void*)topaddr, FILHDRSZ);
        topaddr += FILHDRSZ;

        if (verbose < 1)
                return 0;

        DEBUG_ERROR("\n");
        DEBUG_ERROR("   === FILE Header:\n");
        DEBUG_ERROR("       magic:  0x%04x\n", fhdr.f_magic);
        DEBUG_ERROR("       nscns:  %d", fhdr.f_nscns);
        DEBUG_ERROR("       timdat: 0x%08x\n", fhdr.f_timdat);
        DEBUG_ERROR("       symptr: 0x%08x\n", fhdr.f_symptr);
        DEBUG_ERROR("       nsyms:  0x%08x\n", fhdr.f_nsyms);
        DEBUG_ERROR("       opthdr: %d bytes\n", fhdr.f_opthdr);
        DEBUG_ERROR("       flags:  0x%04x\n", fhdr.f_flags);
        DEBUG_ERROR("       target: 0x%04x\n", fhdr.f_target_id);
        DEBUG_ERROR("\n");

        if (fhdr.f_magic != 0xc2 || fhdr.f_target_id != 0x99) {
                DEBUG_ERROR("** unexpected values in header (not a C6000 COFF file?)\n");

                return -1;
        }

        strbase = fhdr.f_symptr + fhdr.f_nsyms * SYMESZ;

        if (fhdr.f_opthdr && fhdr.f_opthdr != AOUTHDRSZ) {
                DEBUG_ERROR("** unexpected (%d) optional header size\n", fhdr.f_opthdr);

                return -1;
        }

        if (fhdr.f_opthdr) {
                memcpy(&ohdr, (void*)topaddr, AOUTHDRSZ);
                topaddr += AOUTHDRSZ;

                DEBUG_ERROR("   === Optional Header:\n");
                DEBUG_ERROR("       magic:  %04x\n", ohdr.oh_magic);
                DEBUG_ERROR("       vstamp: %04x\n", ohdr.oh_vstamp);
                DEBUG_ERROR("       tsize:  %08x\n", ohdr.oh_tsize);
                DEBUG_ERROR("       dsize:  %08x\n", ohdr.oh_dsize);
                DEBUG_ERROR("       bsize:  %08x\n", ohdr.oh_bsize);
                DEBUG_ERROR("       entry:  %08x\n", ohdr.oh_entry);
                DEBUG_ERROR("       tstart: %08x\n", ohdr.oh_text_start);
                DEBUG_ERROR("       dstart: %08x\n", ohdr.oh_data_start);
                DEBUG_ERROR("\n");
        }

        nb = fhdr.f_nscns * SCNHDRSZ;
        shdr = (COFF_SectionHeader*)VMALLOC_TRC(nb);
        if (shdr == NULL) {
#ifndef USE_HBSLOG
                printf("malloc(%d) failed!\n", nb);
#else
                hb_slogf_error("malloc(%d) failed!\n", nb);
#endif
                return -1;
        }
        memcpy(shdr, (void*)topaddr, nb);
        topaddr += nb;

        if (verbose < 2)
                return 0;

#ifndef USE_HBSLOG
        printf("   === Section Table:\n");
        printf("\n");

        printf("       name           pa       va   size   data nrel flag  data\n");
        printf("       ------------------------------------------------------------\n");
#else
        hb_slogf_error("   === Section Table:");
        hb_slogf_error("\n");

        hb_slogf_error("       name           pa       va   size   data nrel flag  data");
        hb_slogf_error("       ------------------------------------------------------------");
#endif

        for (i = 0; i < fhdr.f_nscns; i += 1) {
                int useit = (shdr[i].s_flags & 0x90) == 0 && shdr[i].s_size != 0 && shdr[i].s_scnptr != 0;
                union {
                        char name[9];
                        uint32_t addr[2];
                } u;
                const char* np;

                memcpy(u.name, shdr[i].s_name, sizeof(shdr[i].s_name));
                u.name[8] = 0;
                np = (u.name[0] ? u.name : coffGetStr(iaddr, strbase + u.addr[1]));

#ifndef USE_HBSLOG
                printf("    %c  %8s", (useit ? '*' : ' '), np);

#else
                hb_slogf_error("    %c  %8s", (useit ? '*' : ' '), np);
#endif
                if (!u.name[0]) {
#ifndef USE_HBSLOG
                        printf("\n               ");
#else
                        hb_slogf_error("\n               ");
#endif
                }

#ifndef USE_HBSLOG
                /* physical address, aliased s_nlib */
                printf(" %8X", shdr[i].s_paddr);
                /* virtual address */
                printf(" %8X", shdr[i].s_vaddr);
                /* section size */
                printf(" %6X", shdr[i].s_size);
                /* file ptr to raw data for section */
                printf(" %6X", shdr[i].s_scnptr);
                /* number of relocation entries */
                printf(" %4d", shdr[i].s_nreloc);
                /* flags */
                printf(" %4X", shdr[i].s_flags);
#else
                /* physical address, aliased s_nlib */
                hb_slogf_error(" %8X", shdr[i].s_paddr);
                /* virtual address */
                hb_slogf_error(" %8X", shdr[i].s_vaddr);
                /* section size */
                hb_slogf_error(" %6X", shdr[i].s_size);
                /* file ptr to raw data for section */
                hb_slogf_error(" %6X", shdr[i].s_scnptr);
                /* number of relocation entries */
                hb_slogf_error(" %4d", shdr[i].s_nreloc);
                /* flags */
                hb_slogf_error(" %4X", shdr[i].s_flags);
#endif

                if (useit && shdr[i].s_size) {
                        uint32_t data;

                        memmove(&data, (void*)(iaddr + shdr[i].s_scnptr), sizeof(data));
#ifndef USE_HBSLOG
                        printf("  %08X%s", data, (shdr[i].s_size ? " ..." : ""));
#else
                        hb_slogf_error("  %08X%s", data, (shdr[i].s_size ? " ..." : ""));
#endif
                }

#ifndef USE_HBSLOG
                printf("\n");
#else
                hb_slogf_error("\n");
#endif
        }
        VFREE_TRC(shdr);

        return 0;
}

#define BCP_PATTERN0 0x534F4942 /* BIOSpattern 0 */
#define BCP_PATTERN1 0x4C525443 /* BIOSpattern 1 */
#define DSP_BTABLE_MAGIC 0xc6000abc
#define MAGIC_IFS 0x00ff7eeb
#define MAGIC_EFS 0xFF4C0010
#define MAGIC_FPGA 0x4744482e
#define MAGIC_BGZ 0x325A4742
#define MAGIC_SCRIPT0 0x61747340
#define MAGIC_SCRIPT1 0x00007472

/**
 * scan flash for ifs, boot tables, scripts, bgz files, ...
 *
 * @param   iaddr    address of the COFF file
 * @return  error code
 */
int fileScan(unsigned param)
{
#if (SW_FLASH == 1)
        flash_get_base_address(0, &flash_base);
        printf("flash scan for %d sectors:\n", flashinfo[0].seccount);

        if ((HW_version & 0xFF) == 0xB1)
                sec = 232;
        addr = (uint32_t*)(flash_base + flashinfo[0].sector[sec].addr); /* set to */
        if ((addr[120] == 0x2D6C7069) && ((addr[121] == 0x3143414A) || (addr[121] == 0x3543414A)))
                printf(" 0x%08x (0x%08x) => ipl %06x: id=%08x opt=%08x\n", addr, addr[123], addr[124], addr[119], addr[125]);
        else
                printf(" 0x%08x (0x........) => unknown or old ipl !?\n", addr);
        for (n = 0; n < flashinfo[0].seccount; n++) {
                addr = (uint32_t*)(flash_base + flashinfo[0].sector[n].addr);
                kill = 0;

                result = coff_file_size(addr);
                if (result > 0) {
                        printf(" 0x%08x (0x%08x) => COFF file\n", addr, result);
                        kill = 1;
                } else {
                        switch (addr[0]) {
                        case BCP_PATTERN0:
                                if (addr[1] == BCP_PATTERN1) {
                                        printf(" 0x%08x (0x%08x) => HBBIOS packet %s \n", addr, swapLong(addr[10]), &addr[24]);
                                        kill = 1;
                                }
                                break;
                        case DSP_BTABLE_MAGIC:
                                printf(" 0x%08x (0x%08x) => DSP boot table (uncompressed: 0x%08x): vers=%08x\n", addr, addr[1], addr[2], addr[4]);
                                if (param > 2) {
                                        *(uint32_t*)param = addr;
                                        param = 2;
                                }
                                kill = 1;
                                break;
                        case MAGIC_IFS:
                                printf(" 0x%08x (0x%08x) => ifs\n", addr, addr[9]);
                                kill = 1;
                                break;
                        case MAGIC_FPGA:
                                printf(" 0x%08x => FPGA\n", addr);
                                break;
                        case MAGIC_EFS:
                                printf(" 0x%08x => efs\n", addr);
                                break;
                        case MAGIC_BGZ:
                                printf(" 0x%08x (0x%08x) => BGZ (uncompressed: 0x%08x)\n", addr, swapLong(addr[1]), swapLong(addr[2]));
                                break;
                        case MAGIC_SCRIPT0:
                                if ((addr[1] & 0xFFFF) == MAGIC_SCRIPT1) {
                                        printf(" 0x%08x (0x%08x) => script\n", addr, flashinfo[0].sector[n].size);
                                        kill = 1;
                                }
                        default:
                                if (addr[2] == MAGIC_IFS) {
                                        printf(" 0x%08x (0x%08x) => ifs\n", addr, addr[11]);
                                        kill = 1;
                                }
                                break;
                        }
                }
                if (kill && (param == 1)) {
                        flash_sector_erase(0, flashinfo[0].sector[n].addr);
                }
        }
        if (param > 2) {
                printf("error: no DSP image found!\n");
                return (ERROR_C6XXX + 0x08);
        } else
                return (NO_ERROR);
#else
        return (-1);
#endif
}

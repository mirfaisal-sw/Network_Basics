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

#define USE_DRA7XX_MEM_ACC

#define USE_HBSLOG
#ifdef USE_HBSLOG
#endif

#include "dspipc-dra7xx.h"
/**
 * Define the chipset handle type to the dra7xx handle type
 */
#define dspIpcChipset_t dspIpcDra7xx_t

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

uint32_t load_elf_file(uint32_t iaddr, int verbose);

extern int dra7xxAccessMem32(dspIpcChipset_t* pChipset,
    uint32_t addr,
    uint32_t numLongWords,
    memAccessDir_t dir,
    uint32_t* pVal);

extern int dra7xxAccessMem8(dspIpcChipset_t* pChipset,
    uint32_t addr,
    uint32_t numBytes,
    memAccessDir_t dir,
    uint8_t* pVal);

#define FALSE 0
#define TURE 1
#define MAX_SIZE 52

#define ProgrameHeaderSize sizeof(Elf32_Phdr)

uint32_t load_elf_file(uint32_t iaddr, int verbose)
{
        Elf32_Ehdr file_header;
        Elf32_Phdr* programe_header_ptr;

        uint32_t topaddr = iaddr;

        ssize_t nb, nl;
        int useit, i;

        memcpy(&file_header, (void*)topaddr, 52);
        topaddr += 52;

        if ((file_header.e_ident[0] == 0x7f) && (file_header.e_ident[1] == 'E') && (file_header.e_ident[2] == 'L') && (file_header.e_ident[3] == 'F')) {
                hb_slogf_error("This is ELF file.\n");

                if (verbose > 1) {
                        hb_slogf_error("file type:");
                        switch (file_header.e_ident[4]) {
                        case 0:
                                hb_slogf_error("invalid object file\n");
                                break;
                        case 1:
                                hb_slogf_error("32-bit object file\n");
                                break;
                        case 2:
                                hb_slogf_error("64-bit object file\n");
                                break;
                        default:
                                break;
                        }

                        hb_slogf_error("coding format:");
                        switch (file_header.e_ident[5]) {
                        case 0:
                                hb_slogf_error("invalid coding format\n");
                                break;
                        case 1:
                                hb_slogf_error("small coding format\n");
                                break;
                        case 2:
                                hb_slogf_error("big coding format\n");
                                break;
                        default:
                                break;
                        }

                        hb_slogf_error("version:");
                        if (file_header.e_ident[6] == 1) {
                                hb_slogf_error("current version\n");
                        } else {
                                hb_slogf_error("NULL\n");
                        }

                        hb_slogf_error("object file type:");
                        switch (file_header.e_type) {
                        case 0:
                                hb_slogf_error("unknown file\n");
                                break;
                        case 1:
                                hb_slogf_error("relocatable file\n");
                                break;
                        case 2:
                                hb_slogf_error("excutable file\n");
                                break;
                        case 3:
                                hb_slogf_error("dll file\n");
                                break;
                        case 4:
                                hb_slogf_error("CORE file\n");
                                break;
                        default:
                                break;
                        }

                        hb_slogf_error("architecture:");
                        switch (file_header.e_machine) {
                        case 0:
                                hb_slogf_error("unkown\n");
                                break;
                        case 3:
                                hb_slogf_error("Intel 8086\n");
                                break;
                        default:
                                break;
                        }

                        hb_slogf_error("program entry point(vitual):");
                        hb_slogf_error("0x%x\n", file_header.e_entry);

                        hb_slogf_error("Start of program headers:");

                        hb_slogf_error("0x%x, %d\n", file_header.e_phoff, file_header.e_phoff);

                        hb_slogf_error("Start of section headers:");

                        hb_slogf_error("0x%x, %d\n", file_header.e_shoff, file_header.e_shoff);

                        hb_slogf_error("Flags:");
                        hb_slogf_error("%d\n", file_header.e_flags);

                        hb_slogf_error("Size of this header:");

                        hb_slogf_error("0x%x, %d\n", file_header.e_ehsize, file_header.e_ehsize);

                        hb_slogf_error("Size of program headers:");

                        hb_slogf_error("0x%x, %d\n", file_header.e_phentsize, file_header.e_phentsize);

                        hb_slogf_error("Number of program headers:");

                        hb_slogf_error("0x%x, %d\n", file_header.e_phnum, file_header.e_phnum);

                        hb_slogf_error("Size of section headers:");

                        hb_slogf_error("0x%x, %d\n", file_header.e_shentsize, file_header.e_shentsize);

                        hb_slogf_error("Number of section headers:");

                        hb_slogf_error("0x%x, %d\n", file_header.e_shnum, file_header.e_shnum);

                        hb_slogf_error("Section header string table index:");

                        hb_slogf_error("0x%x, %d\n", file_header.e_shstrndx, file_header.e_shstrndx);
                }

        } else {

                hb_slogf_error("File NO ELF!\n");
                return FALSE;
        }

        nb = file_header.e_phnum * ProgrameHeaderSize;
        programe_header_ptr = (Elf32_Phdr*)VMALLOC_TRC(nb);

        if (programe_header_ptr == NULL) {
                hb_slogf_error("malloc(%d) failed!", nb);
                mdelay(2);
                return -1;
        }
        memcpy(programe_header_ptr, (void*)(file_header.e_phoff + iaddr), nb);

        for (i = 0, nl = 0; i < file_header.e_phnum; i += 1) {

                if (verbose > 1) {
                        hb_slogf_error("$$$program header table(%d):", i);
                        hb_slogf_error("p_type:%d", programe_header_ptr[i].p_type);
                        hb_slogf_error("p_offset:%d", programe_header_ptr[i].p_offset);
                        hb_slogf_error("p_vaddr:0x%x", programe_header_ptr[i].p_vaddr);
                        hb_slogf_error("p_paddr:0x%x", programe_header_ptr[i].p_paddr);
                        hb_slogf_error("p_filesz:%d", programe_header_ptr[i].p_filesz);
                        hb_slogf_error("p_memsz:%d", programe_header_ptr[i].p_memsz);
                        hb_slogf_error("p_flags:%d", programe_header_ptr[i].p_flags);
                        hb_slogf_error("p_align:%d", programe_header_ptr[i].p_align);
                }

                useit = (void*)programe_header_ptr[i].p_vaddr != NULL && programe_header_ptr[i].p_filesz != 0 && programe_header_ptr[i].p_offset != 0;

                if (useit) {
                        uintptr_t dst = programe_header_ptr[i].p_vaddr;
                        uintptr_t src = programe_header_ptr[i].p_offset + iaddr;
                        uintptr_t len = programe_header_ptr[i].p_filesz;

                        if (verbose > 1) {
#ifndef USE_HBSLOG
                                printf("-- COPY: %8x -> %8x..%08x\n", src, dst, dst + len - 1);
#else
                                hb_slogf_error("-- COPY: %8x -> %8x..%08x", (unsigned int)src, (unsigned int)dst, (unsigned int)(dst + len - 1));
                                mdelay(2);
#endif
                        }

                        if (dst >= 0x800000 && dst < 0x1000000) /* in DSP L1,L2 RAM? */
                                dst += 0x40000000;              /* adjust ARM address view */

#ifndef USE_DRA7XX_MEM_ACC
                        memmove((void*)dst, (const void*)src, len);
#else
                        if (dra7xxAccessMem8(pChipset_Coff,
                                dst,
                                (len),
                                MEM_WRITE,
                                (uint8_t*)src)
                            != 0) {
#ifndef USE_HBSLOG
                                printf("%s: Write failed", __FUNCTION__);
#else
                                hb_slogf_error("%s: Write failed", __FUNCTION__);
#endif
                        }
#endif

                        if (verbose > 1) {
#ifndef USE_HBSLOG
                                printf("%s%x", (nl ? "+" : ""), len);
#else
                                hb_slogf_error("%s0x%x", (nl ? "+" : ""), (unsigned int)len);
                                mdelay(2);
#endif
                        }
                        nl += 1;
                }
        }
        VFREE_TRC(programe_header_ptr);

        return (uint32_t)file_header.e_entry;
}

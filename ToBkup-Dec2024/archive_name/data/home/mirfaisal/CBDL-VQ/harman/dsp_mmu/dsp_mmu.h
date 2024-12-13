#ifndef __TI_DSP_MMU_HANDLER_H_
#define __TI_DSP_MMU_HANDLER_H_

/*
 * Enumerated type used to specify page size
 */
#define TRUE	0x1
#define FALSE	0x0

/* HW bits */
#define MMU_CAM_VATAG_MASK      ((uint32_t)0xfffff000UL)
#define MMU_CAM_VATAG_SHIFT     ((uint32_t)12U)
#define MMU_CAM_P_SHIFT         ((uint32_t)0x3U)
#define MMU_CAM_V_SHIFT         ((uint32_t)0x2U)

#define MMU_RAM_PHYADDR_MASK     ((uint32_t)0xfffff000UL)

#define MMU_RAM_ENDIANNESS_SHIFT ((uint32_t)9UL)
#define MMU_RAM_ENDIANNESS_MASK  ((uint32_t)0x000000200UL)

#define MMU_RAM_MIXED_SHIFT      ((uint32_t)6UL)
#define MMU_RAM_MIXED_MASK       ((uint32_t)0x00000040UL)

#define MMU_RAM_ELEMENTSZ_SHIFT  ((uint32_t)7UL)
#define MMU_RAM_ELEMENTSZ_MASK   ((uint32_t)0x00000180UL)


#define	MMU_PAGESIZE_SECTION_1MB	((uint32_t)0UL)
#define	MMU_PAGESIZE_LARGE_64KB		((uint32_t)1UL)
#define	MMU_PAGESIZE_SMALL_4KB		((uint32_t)2UL)
#define	MMU_PAGESIZE_SUPER_16MB		((uint32_t)3UL)

typedef enum mmu_page_sz
{
	MMU_SECTION_SZ	= MMU_PAGESIZE_SECTION_1MB,
	MMU_LARGE_SZ 	= MMU_PAGESIZE_LARGE_64KB,
	MMU_SMALL_SZ 	= MMU_PAGESIZE_SMALL_4KB,
	MMU_SUPER_SZ 	= MMU_PAGESIZE_SUPER_16MB
} mmu_page_sz_t;

/*
 * Endianness of mmu
 */ 
typedef enum mmu_endianness
{
	MMU_LITTLE_ENDIAN	= 0x0,
	MMU_BIG_ENDIAN 		= 0x1
} mmu_endianness_t;

/*
 * mmu element size
 */
typedef enum mmu_element_sz
{
	MMU_ELM_SZ_BYTE			= 0x0,
	MMU_ELM_SZ_SHORT		= 0x1,
	MMU_ELM_SZ_LONG 		= 0x2,
	MMU_ELM_SZ_NOTRANSLATION 	= 0x3
}mmu_element_sz_t;

/*
 * mmu mixed page attribute use
 */
typedef enum mmu_mixpage_attr
{
	MMU_TLB_ELM_SZ = 0x0,
	MMU_CPU_ELM_SZ = 0x1,
}mmu_mixed_page_attr_t;

typedef struct mmu_tlbentry
{
	uint32_t		phyaddr;
	uint32_t		virtaddr;
	uint32_t		valid;

	mmu_page_sz_t		pg_sz;
	mmu_endianness_t	mmu_endian;
	mmu_element_sz_t 	elm_sz;
	mmu_mixed_page_attr_t 	tlb_elm_sz;

	uint32_t		resv;

} mmu_tlbentry_t;

int mmu_set_tlb_entry( uint32_t base_addr, uint32_t tlb,\
			const mmu_tlbentry_t *mmu_tlbentry);

#endif /* endof __TI_DSP_MMU_HANDLER_H_ */	

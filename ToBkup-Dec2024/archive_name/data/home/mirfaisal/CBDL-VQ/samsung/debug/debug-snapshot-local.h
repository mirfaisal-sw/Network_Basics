/* SPDX-License-Identifier: GPL-2.0 */
/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 */

#ifndef DEBUG_SNAPSHOT_LOCAL_H
#define DEBUG_SNAPSHOT_LOCAL_H
#include <soc/samsung/debug-snapshot-log.h>
#include <dt-bindings/soc/samsung/debug-snapshot-def.h>

struct dbg_snapshot_info {
	size_t size;
	size_t vaddr;
	size_t paddr;
	unsigned int enabled;
};

struct dbg_snapshot_base {
	size_t size;
	size_t vaddr;
	size_t paddr;
	int magic;
	unsigned int version;
	unsigned int enabled;
};

struct dbg_snapshot_item {
	char *name;
	struct dbg_snapshot_info entry;
	unsigned int persist;
	unsigned char *head_ptr;
	unsigned char *curr_ptr;
};

struct dbg_snapshot_log_item {
	char *name;
	struct dbg_snapshot_info entry;
};

struct dbg_snapshot_desc {
	struct device *dev;
	raw_spinlock_t ctrl_lock;
	int sjtag_status;
	bool panic_to_wdt;
	bool last_kmsg;
	unsigned int hold_key;
	unsigned int trigger_key;

	u32 mmu_regs_offset;
	u32 core_regs_offset;
};

struct dbg_snapshot_dpm {
	bool enabled;
	unsigned int version;
	bool enabled_debug;
	unsigned int dump_mode;
	int dump_mode_none;
};

struct dbg_snapshot_backtrace_data {
	u64 paddr;
	char *vaddr;
	size_t size;
	u64 curr_idx;
	bool stop_logging;
};

extern size_t dss_print_time(u64 ts, char *buf);
extern inline u64 dss_arch_timer_get_ns(void);
extern int dbg_snapshot_get_pcpu_id(int smp_processor_id);
extern void dbg_snapshot_init_log(void);
extern void dbg_snapshot_init_dpm(void);
extern void dbg_snapshot_init_utils(void);
extern void dbg_snapshot_init_pmsg(void);
extern int dss_pmsg_tty_init(void);
extern void dss_pmsg_tty_exit(void);
extern int dbg_snapshot_dt_scan_dpm(void);
extern int dbg_snapshot_get_enable(void);
extern void dbg_snapshot_output(void);
extern void dbg_snapshot_log_output(void);
extern void dbg_snapshot_hook_logger(const char *buf, size_t size, unsigned int id);
extern void __iomem *dbg_snapshot_get_header_vaddr(void);
extern unsigned long dbg_snapshot_get_header_paddr(void);
extern void dbg_snapshot_scratch_reg(unsigned int val);
extern void dbg_snapshot_print_log_report(void);
extern void dbg_snapshot_set_debug_test_buffer_addr(u64 paddr, unsigned int cpu);
extern unsigned int dbg_snapshot_get_debug_test_buffer_addr(unsigned int cpu);
extern void dbg_snapshot_set_qd_entry(unsigned long address);
extern int dbg_snapshot_get_num_items(void);
extern int dbg_snapshot_log_get_num_items(void);
extern struct dbg_snapshot_item *dbg_snapshot_get_item(const char *name);
extern void *dbg_snapshot_get_item_by_index(int index);
extern void *dbg_snapshot_log_get_item_by_index(int index);
extern void dbg_snapshot_set_enable_log_item(const char *name, int en);
extern int dbg_snapshot_get_dpm_none_dump_mode(void);
extern void dbg_snapshot_set_dpm_none_dump_mode(unsigned int mode);
extern void cache_flush_all(void);


#if IS_ENABLED(CONFIG_TRACE_MMIO_ACCESS)
#define register_rwmmio_write(a, b)	register_trace_rwmmio_write(a, b)
#define register_rwmmio_read(a, b)	register_trace_rwmmio_read(a, b)
#define register_rwmmio_post_read(a, b)	register_trace_rwmmio_post_read(a, b)
#else
#define register_rwmmio_write(a, b)	do { } while (0)
#define register_rwmmio_read(a, b)	do { } while (0)
#define register_rwmmio_post_read(a, b)	do { } while (0)
#endif

extern struct dbg_snapshot_log *dss_log;
extern struct dbg_snapshot_desc dss_desc;
extern struct dbg_snapshot_dpm dss_dpm;

/*  Size domain */
#define DSS_KEEP_HEADER_SZ		(SZ_256 * 3)
#define DSS_HEADER_SZ			SZ_4K //0x0 -- 0x1000

#define DSS_MMU_REG_OFFSET		SZ_512
#define DSS_CORE_REG_OFFSET		SZ_512
#define DSS_MAX_BL_SIZE			(24)

#define DSS_PANIC_STRING_SIZE		(SZ_512)

/* Sign domain */
#define DSS_SIGN_RESET			0x0
#define DSS_SIGN_RESERVED		0x1
#define DSS_SIGN_SCRATCH		0xD
#define DSS_SIGN_ALIVE			0xFACE
#define DSS_SIGN_DEAD			0xDEAD
#define DSS_SIGN_PANIC			0xBABA
#define DSS_SIGN_SAFE_FAULT		0xFAFA
#define DSS_SIGN_NORMAL_REBOOT		0xCAFE
#define DSS_SIGN_LOCKUP			0xDEADBEEF
#define DSS_SIGN_MAGIC			(0xDB9 << 16)
#define DSS_BOOT_CNT_MAGIC		0xFACEDB90
#define DSS_BACKTRACE_MAGIC		0x0DB90DB9
#define DSS_ORDERING_PCPU		0x75706370
#define DSS_ORDERING_VCPU		0x75706376

/*  Specific Address Information */
#define DSS_OFFSET_SCRATCH		(0x100)
#define DSS_OFFSET_NONE_DPM_DUMP_MODE	(0x108)
#define DSS_OFFSET_DEBUG_TEST_BUFFER(n)	(0x190 + (0x8 * n))
#define DSS_OFFSET_LAST_LOGBUF		(0x200)
#define DSS_OFFSET_LAST_PLATFORM_LOGBUF	(0x204)
#define DSS_OFFSET_CHECK_EOB		(0x208)
#define DSS_OFFSET_BL_BOOT_CNT_MAGIC	(0x210)
#define DSS_OFFSET_BL_BOOT_CNT		(0x214)
#define DSS_OFFSET_KERNEL_BOOT_CNT_MAGIC (0x218)
#define DSS_OFFSET_KERNEL_BOOT_CNT	(0x21C)
#define DSS_OFFSET_MEMLOG_BL_BASE	DSS_MEMLOG_BL_BASE /* 0x290 */
#define DSS_OFFSET_MEMLOG_BL_MAGIC	(DSS_MEMLOG_BL_BASE + 0x8) /*0x298 */
#define DSS_OFFSET_BACKTRACE_MAGIC	(0x2A0)
#define DSS_OFFSET_BACKTRACE_CPU	(0x2A4)
#define DSS_OFFSET_BACKTRACE_PADDR	(0x2A8)
#define DSS_OFFSET_BACKTRACE_SIZE	(0x2B0)
#define DSS_OFFSET_EMERGENCY_REASON	(0x300)
#define DSS_OFFSET_WDT_CALLER		(0x310)
#define DSS_OFFSET_CORE_POWER_STAT	(0x400)
#define DSS_OFFSET_PANIC_STAT		(0x500)
#define DSS_OFFSET_CPU_ORDERING		(0x600)

/* DSS_BASE + 0x700 -- 0x8FF is reserved */
#define DSS_OFFSET_LINUX_BANNER		(0x700)
/* DSS_BASE + 0x900 -- 0xDFF is reserved */
#define DSS_OFFSET_ITEM_INFO		(0x900)
/* DSS_BASE + 0xE00 -- 0xFFF is reserved */
#define DSS_OFFSET_PANIC_STRING		(0xE00)

#define DSS_OFFSET_MMU_REGS_DEFAULT	(0x1000)
#define DSS_OFFSET_CORE_REGS_DEFAULT	(0x2000)
#endif

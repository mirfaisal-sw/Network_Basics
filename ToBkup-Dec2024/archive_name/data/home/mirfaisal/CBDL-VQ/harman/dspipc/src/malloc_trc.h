#ifndef __MALLOC_TRC_H__
#define __MALLOC_TRC_H__

void* my_kmalloc(size_t size);
void my_kfree(void* addr);
void* my_kcalloc(size_t num, size_t size);

#define MALLOC_TRC(size) my_kmalloc(size)
#define CALLOC_TRC(num, size) my_kcalloc(num, size)
#define FREE_TRC(addr) my_kfree(addr)

#define MALLOC_TRC_INIT
#define print_mem_trace_info(num)

void* my_vmalloc(size_t size);
void my_vfree(void* addr);

#define VMALLOC_TRC(size) my_vmalloc(size)

#define VFREE_TRC(addr) my_vfree(addr)

#define VIRT_TO_PHYS virt_to_phys

#endif

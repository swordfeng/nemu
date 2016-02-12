#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)
#define NR_VPTE

static PTE vptable[NR_PTE] align_to_page;
/* Use the function to get the start address of user page directory. */
PDE* get_updir();

void create_video_mapping() {
	/* Create an identical mapping from virtual memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
    PDE *updir = get_updir();
    PTE *ptable = (PTE *)va_to_pa(vptable);
    updir[0].val = make_pde(ptable);

    memset(vptable, 0, sizeof(vptable));

    uint32_t vmem_addr = VMEM_ADDR;
    uint32_t idx = VMEM_ADDR / PAGE_SIZE;
    for (vmem_addr = VMEM_ADDR; vmem_addr < VMEM_ADDR + SCR_SIZE; vmem_addr += PAGE_SIZE) {
        vptable[idx].val = make_pte(vmem_addr);
        idx++;
    }
}

void video_mapping_write_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		buf[i] = i;
	}
}

void video_mapping_read_test() {
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for(i = 0; i < SCR_SIZE / 4; i ++) {
		assert(buf[i] == i);
	}
}

void video_mapping_clear() {
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}

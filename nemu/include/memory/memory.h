#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "common.h"

#define HW_MEM_SIZE (128 * 1024 * 1024)

extern uint8_t *hw_mem;

/* convert the hardware address in the test program to virtual address in NEMU */
#define hwa_to_va(p) ((void *)(hw_mem + (unsigned)p))
/* convert the virtual address in NEMU to hardware address in the test program */
#define va_to_hwa(p) ((hwaddr_t)((void *)p - (void *)hw_mem))

#define hw_rw(addr, type) *(type *)({\
    Assert(addr < HW_MEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    hwa_to_va(addr); \
})

#ifdef USE_CACHE
uint32_t cached_read(hwaddr_t addr, size_t len);
void cached_write(hwaddr_t addr, size_t len, uint32_t data);
void cache_show(hwaddr_t addr);
#endif

uint32_t swaddr_read(swaddr_t, size_t, uint8_t);
void swaddr_read_bytes(void *, swaddr_t, size_t, uint8_t);
uint32_t lnaddr_read(lnaddr_t, size_t);
uint32_t hwaddr_read(hwaddr_t, size_t);
void swaddr_write(swaddr_t, size_t, uint8_t, uint32_t);
void lnaddr_write(lnaddr_t, size_t, uint32_t);
void hwaddr_write(hwaddr_t, size_t, uint32_t);

lnaddr_t seg_translate(swaddr_t offset, size_t len, uint8_t sreg);

hwaddr_t page_translate(lnaddr_t);
void page_show(lnaddr_t);

#ifdef DEEP_PERFORMANCE
hwaddr_t page_translate_fast(lnaddr_t lnaddr);
#endif

#ifdef PERFORMANCE
#define NR_TLB_LINE (1 << 20)
extern uint32_t tlb[NR_TLB_LINE];
#endif

#ifndef DEEP_PERFORMANCE
hwaddr_t tlb_translate(lnaddr_t lnaddr);
#else
inline static hwaddr_t tlb_translate(lnaddr_t lnaddr) {
    uint32_t pageid = lnaddr >> 12;
    if (tlb[pageid] == ~0u) {
        tlb[pageid] = page_translate_fast(lnaddr);
    }
    return tlb[pageid] | (lnaddr & 0xFFF);
}
#endif
void tlb_flush();

#endif

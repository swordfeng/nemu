#include "common.h"
#include "memory/memory.h"
#include "cpu/reg.h"
#include <stdlib.h>

#ifdef USE_TLB

#define NR_TLB_LINE 64

typedef struct TLBLine {
    uint32_t tag;
    uint32_t paddr;
    bool valid;
} TLBLine;
TLBLine tlb[NR_TLB_LINE];

hwaddr_t tlb_translate(lnaddr_t lnaddr) {
    uint32_t tag = lnaddr >> 12;
    for (TLBLine *l = tlb; l < tlb + NR_TLB_LINE; l += sizeof(TLBLine)) {
        if (l->valid && l -> tag == tag) return l->paddr | (lnaddr & 0xFFF);
    }
    hwaddr_t hwaddr = page_translate(lnaddr);
    size_t idx = rand() & (NR_TLB_LINE - 1);
    tlb[idx].valid = true;
    tlb[idx].tag = tag;
    tlb[idx].paddr = hwaddr & ~0xFFF;
    return hwaddr;
}

void tlb_flush() {
    for (TLBLine *l = tlb; l < tlb + NR_TLB_LINE; l += sizeof(TLBLine)) {
        l->valid = false;
    }
}
#endif

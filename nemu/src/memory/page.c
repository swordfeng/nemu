#include "common.h"
#include "memory/memory.h"
#include "cpu/reg.h"

extern jmp_buf mainloop_jbuf;

hwaddr_t page_translate(lnaddr_t lnaddr) {
    hwaddr_t pde_addr = (cpu.cr3 & 0xFFFFF000) + ((lnaddr >> 20) & 0xFFC);
    uint32_t pde = hwaddr_read(pde_addr, 4);
#ifndef DEEP_PERFORMANCE
    vm_assert(pde & 1, "PDE not present, address = %#10x", lnaddr);
    vm_assert(((pde >> 7) & 1) == 0, "4MB page not supported");
#endif
    hwaddr_t pte_addr = (pde & 0xFFFFF000) + ((lnaddr >> 10) & 0xFFC);
    uint32_t pte = hwaddr_read(pte_addr, 4);
#ifndef DEEP_PERFORMANCE
    vm_assert(pte & 1, "PTE not present, address = %#10x", lnaddr);
#endif
    hwaddr_t hwaddr = (pte & 0xFFFFF000) | (lnaddr & 0xFFF);
    return hwaddr;
}

#ifdef DEEP_PERFORMANCE
hwaddr_t page_translate_fast(lnaddr_t lnaddr) {
    hwaddr_t pde_addr = (cpu.cr3 & 0xFFFFF000) + ((lnaddr >> 20) & 0xFFC);
    uint32_t pde = hwaddr_read(pde_addr, 4);
    hwaddr_t pte_addr = (pde & 0xFFFFF000) + ((lnaddr >> 10) & 0xFFC);
    uint32_t pte = hwaddr_read(pte_addr, 4);
    return pte & 0xFFFFF000;
}
#endif

void page_show(lnaddr_t lnaddr) {
    if (!cpu.cr0.pg) {
        printf("Paging disabled\n");
    } else {
        hwaddr_t pde_addr = (cpu.cr3 & 0xFFFFF000) + ((lnaddr >> 20) & 0xFFC);
        uint32_t pde = hwaddr_read(pde_addr, 4);
        printf("PDE:\t%#10x\n", pde);
        if ((pde & 1) == 0) {
            printf("PDE invalid, address = %#10x\n", lnaddr);
            return;
        }
        Assert(((pde >> 7) & 1) == 0, "4MB page not supported");
        hwaddr_t pte_addr = (pde & 0xFFFFF000) + ((lnaddr >> 10) & 0xFFC);
        uint32_t pte = hwaddr_read(pte_addr, 4);
        printf("PTE:\t%#10x\n", pte);
        if ((pte & 1) == 0) {
            printf("PTE invalid, address = %#10x\n", lnaddr);
            return;
        }
        hwaddr_t hwaddr = (pte & 0xFFFFF000) + (lnaddr & 0xFFF);
        printf("hwaddr:\t%#10x\n", hwaddr);
    }
}

#include "common.h"
#include "memory/memory.h"
#include "cpu/reg.h"

hwaddr_t page_translate(lnaddr_t lnaddr) {
    hwaddr_t pde_addr = (cpu.cr3 & 0xFFFFF000) + ((lnaddr >> 20) & 0xFFC);
    uint32_t pde = hwaddr_read(pde_addr, 4);
    Assert(pde & 1, "PDE not present");
    Assert(((pde >> 7) & 1) == 0, "4MB page not supported");
    hwaddr_t pte_addr = (pde & 0xFFFFF000) + ((lnaddr >> 10) & 0xFFC);
    uint32_t pte = hwaddr_read(pte_addr, 4);
    Assert(pte & 1, "PTE not present");
    hwaddr_t hwaddr = (pte & 0xFFFFF000) | (lnaddr & 0xFFF);
    return hwaddr;
}

void page_show(lnaddr_t lnaddr) {
    if (!cpu.cr0.pg) {
        printf("Paging disabled\n");
    } else {
        hwaddr_t pde_addr = (cpu.cr3 & 0xFFFFF000) + ((lnaddr >> 20) & 0xFFC);
        uint32_t pde = hwaddr_read(pde_addr, 4);
        printf("PDE:\t%#10x\n", pde);
        if ((pde & 1) == 0) {
            printf("PDE invalid\n");
            return;
        }
        Assert(((pde >> 7) & 1) == 0, "4MB page not supported");
        hwaddr_t pte_addr = (pde & 0xFFFFF000) + ((lnaddr >> 10) & 0xFFC);
        uint32_t pte = hwaddr_read(pte_addr, 4);
        printf("PTE:\t%#10x\n", pte);
        if ((pte & 1) == 0) {
            printf("PDE invalid\n");
            return;
        }
        hwaddr_t hwaddr = (pte & 0xFFFFF000) + (lnaddr & 0xFFF);
        printf("hwaddr:\t%#10x\n", hwaddr);
    }
}

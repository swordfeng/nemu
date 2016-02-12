#include "common.h"
#include "memory/memory.h"
#include "cpu/reg.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
#ifdef USE_CACHE
    return cached_read(addr, len);
#else
    return dram_read(addr, len);
#endif
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
#ifdef USE_CACHE
    cached_write(addr, len, data);
#else
    dram_write(addr, len, data);
#endif
}

hwaddr_t page_translate(lnaddr_t lnaddr) {
    if (!cpu.cr0.pg) return lnaddr;
    hwaddr_t pde_addr = (cpu.cr3 & 0xFFFFF000) + ((lnaddr >> 20) & 0xFFC);
    uint32_t pde = hwaddr_read(pde_addr, 4);
    Assert(pde & 1, "PDE not present");
    Assert(((pde >> 7) & 1) == 0, "4MB page not supported");
    hwaddr_t pte_addr = (pde & 0xFFFFF000) + ((lnaddr >> 10) & 0xFFC);
    uint32_t pte = hwaddr_read(pte_addr, 4);
    Assert(pte & 1, "PTE not present");
    hwaddr_t hwaddr = (pte & 0xFFFFF000) + (lnaddr & 0xFFF);
    return hwaddr;
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
    size_t max_len = 0x1000 - (addr & 0xFFF);
    if (len > max_len) {
        // data cross the page boundary
        uint32_t low = lnaddr_read(addr, max_len);
        uint32_t high = lnaddr_read(addr + max_len, len - max_len);
        return (high << (max_len * 8)) | low;
    }
    hwaddr_t hwaddr = page_translate(addr);
    return hwaddr_read(hwaddr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
    size_t max_len = 0x1000 - (addr & 0xFFF);
    if (len > max_len) {
        // data cross the page boundary
        lnaddr_write(addr, max_len, data & ((1 << (max_len * 8)) - 1));
        lnaddr_write(addr + max_len, len - max_len, data >> (max_len * 8));
        return;
    }
    hwaddr_t hwaddr = page_translate(addr);
    hwaddr_write(hwaddr, len, data);
}

lnaddr_t seg_translate(swaddr_t offset, size_t len, uint8_t sreg) {
    if (!cpu.cr0.pe) return offset;
    Assert(sreg < 6, "invalid sreg");
    uint16_t segsel = cpu.sr[sreg].sel;
    uint16_t index = segsel >> 3;
    uint8_t ti = (segsel >> 2) & 1;
    //uint8_t rpl = segsel & 3;

    Assert(ti == 0, "ldt not implemented");
//    Assert(index != 0, "null segment selector (#GP)");

    Assert(index * 8 < cpu.gdtr.limit, "gdt index out of range");

    //Assert(dpl >= rpl, "dpl >= rpl");
    Assert(offset < cpu.sr[sreg].limit && offset + len < cpu.sr[sreg].limit, "out of segment");

//    if (cpu.sr[sreg].base != 0) printf("translate to %s, base is %x\n", reg_seg_get_name(sreg), cpu.sr[sreg].base);

    return cpu.sr[sreg].base + offset;
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
    assert(len == 1 || len == 2 || len == 4);
#endif
    lnaddr_t lnaddr = seg_translate(addr, len, sreg);
    return lnaddr_read(lnaddr, len);
}

void swaddr_read_bytes(void *dest, swaddr_t addr, size_t len, uint8_t sreg) {
    while (len >= 4) {
        *(uint32_t *)dest = swaddr_read(addr, 4, sreg);
        dest += 4;
        addr += 4;
        len -= 4;
    }
    while (len > 1) {
        *(uint8_t *)dest = swaddr_read(addr, 1, sreg);
        dest += 1;
        addr += 1;
        len -= 1;
    }
}

void swaddr_write(swaddr_t addr, size_t len, uint8_t sreg, uint32_t data) {
#ifdef DEBUG
    assert(len == 1 || len == 2 || len == 4);
#endif
    lnaddr_t lnaddr = seg_translate(addr, len, sreg);
    lnaddr_write(lnaddr, len, data);
}

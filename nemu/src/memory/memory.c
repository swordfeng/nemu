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

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
    return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
    hwaddr_write(addr, len, data);
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

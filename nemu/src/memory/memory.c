#include "common.h"
#include "memory/memory.h"
#include "cpu/reg.h"
#include "device/mmio.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
    int map_id = is_mmio(addr);
    if (map_id >= 0) {
        return mmio_read(addr, len, map_id);
    } else {
#ifdef USE_CACHE
        return cached_read(addr, len);
#else
        return dram_read(addr, len);
#endif
    }
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
    int map_id = is_mmio(addr);
    if (map_id >= 0) {
        mmio_write(addr, len, data, map_id);
    } else {
#ifdef USE_CACHE
        cached_write(addr, len, data);
#else
        dram_write(addr, len, data);
#endif
    }
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
    size_t max_len = ((~addr) & 0xFFF) + 1;
    if (len > max_len) {
        // data cross the page boundary
        uint32_t low = lnaddr_read(addr, max_len);
        uint32_t high = lnaddr_read(addr + max_len, len - max_len);
        return (high << (max_len * 8)) | low;
    }
    hwaddr_t hwaddr;
    if (cpu.cr0.pg) {
#ifdef USE_TLB
        hwaddr = tlb_translate(addr);
#else
        hwaddr = page_translate(addr);
#endif
    } else {
        hwaddr = addr;
    }
    return hwaddr_read(hwaddr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
    size_t max_len = ((~addr) & 0xFFF) + 1;
    if (len > max_len) {
        // data cross the page boundary
        lnaddr_write(addr, max_len, data & ((1 << (max_len * 8)) - 1));
        lnaddr_write(addr + max_len, len - max_len, data >> (max_len * 8));
        return;
    }
    hwaddr_t hwaddr;
    if (cpu.cr0.pg) {
#ifdef USE_TLB
    hwaddr = tlb_translate(addr);
#else
    hwaddr = page_translate(addr);
#endif
    } else {
        hwaddr = addr;
    }
    hwaddr_write(hwaddr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
    assert(len == 1 || len == 2 || len == 4);
#endif
    lnaddr_t lnaddr;
    if (cpu.cr0.pe) {
        lnaddr = seg_translate(addr, len, sreg);
    } else {
        lnaddr = addr;
    }
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
    lnaddr_t lnaddr;
    if (cpu.cr0.pe) {
        lnaddr = seg_translate(addr, len, sreg);
    } else {
        lnaddr = addr;
    }
    lnaddr_write(lnaddr, len, data);
}

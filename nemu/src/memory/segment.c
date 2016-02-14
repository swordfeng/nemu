#include "common.h"
#include "memory/memory.h"
#include "cpu/reg.h"


lnaddr_t seg_translate(swaddr_t offset, size_t len, uint8_t sreg) {
    Assert(sreg < 6, "invalid sreg");
    uint16_t segsel = cpu.sr[sreg].sel;
    uint16_t index = segsel >> 3;
    uint8_t ti = (segsel >> 2) & 1;
    //uint8_t rpl = segsel & 3;

    Assert(ti == 0, "ldt not implemented");
    //    Assert(index != 0, "null segment selector (#GP)");

    Assert(index <= cpu.gdtr.limit, "gdt index out of range");

    //Assert(dpl >= rpl, "dpl >= rpl");
    Assert(offset < cpu.sr[sreg].limit && offset + len < cpu.sr[sreg].limit, "out of segment");

    return cpu.sr[sreg].base + offset;
}

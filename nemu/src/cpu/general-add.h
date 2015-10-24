#pragma once

static inline uint32_t general_add(uint32_t val1, uint32_t val2, size_t size) {
    uint32_t result = signed_extend(val1 + val2 + cpu.cf, size);
    cpu.cf ^= result < val1 || (cpu.cf && result == val1);
    cpu.zf = result == 0;
    cpu.sf = result >> 31;
    cpu.of = (val1 >> 31) == (val2 >> 31) && (val1 >> 31) != cpu.sf;
    cpu.pf = calc_pf(result);
    return result;
}

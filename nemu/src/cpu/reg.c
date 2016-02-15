#include "nemu.h"
#include "memory/memory.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>

CPU_state cpu;

const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};

const char *regss[] = {"es", "cs", "ss", "ds", "fs", "gs"};

void reg_test() {
    srand(time(0));
    uint32_t sample[8];
    uint32_t eip_sample = rand();
    cpu.eip = eip_sample;

    int i;
    for(i = R_EAX; i <= R_EDI; i ++) {
        sample[i] = rand();
        reg_l(i) = sample[i];
        assert(reg_w(i) == (sample[i] & 0xffff));
    }

    assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
    assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
    assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
    assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
    assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
    assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
    assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
    assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

    assert(sample[R_EAX] == cpu.eax);
    assert(sample[R_ECX] == cpu.ecx);
    assert(sample[R_EDX] == cpu.edx);
    assert(sample[R_EBX] == cpu.ebx);
    assert(sample[R_ESP] == cpu.esp);
    assert(sample[R_EBP] == cpu.ebp);
    assert(sample[R_ESI] == cpu.esi);
    assert(sample[R_EDI] == cpu.edi);

    assert(eip_sample == cpu.eip);
}

const char *reg_get_name(int reg_index, size_t size) {
    switch (size) {
    case 1:
        return regsb[reg_index];
    case 2:
        return regsw[reg_index];
    case 4:
        return regsl[reg_index];
    default:
        panic("wrong reg size!");
    }
}

const char *reg_seg_get_name(int sreg_index) {
    return regss[sreg_index];
}

uint32_t reg_name_mask(const char *name) {
    int index;
    for (index = 0; index < 8; index++) {
        if (strcmp(name, regsw[index]) == 0) return 0xffff;
        if (strcmp(name, regsb[index]) == 0) return 0xff;
    }
    for (index = 0; index < 6; index++) {
        if (strcmp(name, regss[index]) == 0) return 0xffff;
    }
    return 0xffffffff;
}

uint32_t *reg_name_ptr(const char *name) {
    int index;
    for (index = 0; index < 8; index++) {
        if (strcmp(name, regsl[index]) == 0) return &reg_l(index);
        if (strcmp(name, regsw[index]) == 0) return (uint32_t *)&reg_w(index);
        if (strcmp(name, regsb[index]) == 0) return (uint32_t *)&reg_b(index);
    }
    if (strcmp(name, "eflags") == 0) return &cpu.eflags;
    if (strcmp(name, "eip") == 0) return &cpu.eip;
    for (index = 0; index < 6; index++) {
        if (strcmp(name, regss[index]) == 0) return (uint32_t *)&cpu.sr[index].sel;
    }
    if (strcmp(name, "cr0") == 0) return &cpu.cr0.value;
    if (strcmp(name, "cr3") == 0) return &cpu.cr3;
    panic("invalid register name");
}

void init_reg() {
    /* Set the initial eflags. */
    cpu.eflags = 0x2; // According to Intel Manual

    cpu.gdtr.base = 0;
    cpu.gdtr.limit = 0xFFFF;
    cpu.idtr.base = 0;
    cpu.idtr.limit = 0xFFFF;
    cpu.cr0.value = 0;
    cpu.cs.base = 0;
    cpu.cs.limit = 0xffffffff;
}

void sreg_load(uint8_t sreg) {
    Assert(cpu.cr0.pe, "Not in protection mode");
    uint16_t segsel = cpu.sr[sreg].sel;
    uint16_t index = segsel >> 3;
    uint8_t ti = (segsel >> 2) & 1;
    //uint8_t rpl = segsel & 3;

    Assert(ti == 0, "LDT is not supported");
    Assert(index * 8 < cpu.gdtr.limit, "segment selector out of limit");

    uint64_t descriptor = ((uint64_t) lnaddr_read(cpu.gdtr.base + index * 8 + 4, 4) << 32) | lnaddr_read(cpu.gdtr.base + index * 8, 4);

    uint32_t limit = (descriptor & 0xFFFF) | ((descriptor >> 32) & 0xF0000);
    uint32_t base = ((descriptor >> 16) & 0xFFFFFF) | ((descriptor >> 32) & 0xFF000000);
    //uint8_t dpl = (decriptor >> 45) & 3;
    uint8_t granularity = (descriptor >> 55);
    if (granularity) {
        limit <<= 12;
    }

    cpu.sr[sreg].base = base;
    cpu.sr[sreg].limit = limit;
}

uint32_t reg_read_index(uint8_t reg_index, size_t size) {
    switch (size) {
    case 1:
        return reg_b(reg_index);
    case 2:
        return reg_w(reg_index);
    case 4:
        return reg_l(reg_index);
    default:
        panic("wrong reg size!");
    }
}

uint32_t reg_cr_read_index(uint8_t reg_index) {
    switch (reg_index) {
        case 0:
            return cpu.cr0.value;
        case 3:
            return cpu.cr3;
        default:
            panic("cr not implemented");
    }
}

void reg_cr_set(uint8_t reg_index, uint32_t value) {
    switch (reg_index) {
        case 0:
            cpu.cr0.value ^= value;
#ifdef USE_TLB
            if (cpu.cr0.pg) tlb_flush();
#endif
            cpu.cr0.value = value;
            break;
        case 3:
#ifdef USE_TLB
            tlb_flush();
#endif
            cpu.cr3 = value;
            break;
        default:
            panic("cr not implemented");
    }
}

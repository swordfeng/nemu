#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

struct SR {
    uint16_t sel;
    uint32_t base;
    uint32_t limit;
//    uint16_t access;
};

typedef struct {
    // GPRs
    union {
        union {
            uint32_t _32;
            uint16_t _16;
            uint8_t _8[2];
        } gpr[8];
        struct {
            /* Do NOT change the order of the GPRs' definitions. */
            uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
        };
    };
    // SRs
    union {
        struct SR sr[6];
        struct {
            /* Do NOT change the order */
            struct SR es, cs, ss, ds, fs, gs;
        };
    };
    // EFLAGS
    // We only care CF, PF, ZF, SF, IF, DF, OF
    struct {
        union {
            uint32_t eflags;
            struct {
                uint8_t cf : 1;
                uint8_t __reserved1 : 1;
                uint8_t pf : 1;
                uint8_t __reserved2 : 1;
                uint8_t af : 1;
                uint8_t __reserved3 : 1;
                uint8_t zf : 1;
                uint8_t sf : 1;
                uint8_t tf : 1;
                uint8_t ief : 1; /* if */
                uint8_t df : 1;
                uint8_t of : 1;
                uint8_t iopl : 1;
                uint8_t nt : 1;
                uint8_t __reserved4 : 1;
                uint8_t rf : 1;
                uint8_t vm : 1;
                uint8_t ac : 1;
                uint8_t vif : 1;
                uint8_t vip : 1;
                uint8_t id : 1;
            };
        };
    };
    // EIP
    swaddr_t eip;
    // GDTR, IDTR
    struct {
        lnaddr_t base;
        uint16_t limit;
    } gdtr;
    union {
        uint32_t value;
        struct {
            uint8_t pe : 1;
            uint8_t mp : 1;
            uint8_t em : 1;
            uint8_t ts : 1;
            uint8_t et : 1;
            uint8_t ne : 1;
            uint16_t __reserved5 : 10;
            uint8_t wp : 1;
            uint8_t __reserved6: 1;
            uint8_t am : 1;
            uint16_t __reserved7: 10;
            uint8_t nw : 1;
            uint8_t cd : 1;
            uint8_t pg : 1;
        };
    } cr0;
} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
    assert(index >= 0 && index < 8);
    return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

const char *reg_get_name(int reg_index, size_t size);
const char *reg_seg_get_name(int sreg_index);
uint32_t reg_name_mask(const char *name);
uint32_t *reg_name_ptr(const char *name);
void init_reg();

void sreg_load(uint8_t sreg);

#define sreg_index(name) (&cpu.name - cpu.sr)

#endif

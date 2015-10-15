#ifndef __REG_H__
#define __REG_H__

#include "common.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

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
	// EFLAGS
	// We only care CF, PF, ZF, SF, IF, DF, OF
	struct {
		union {
			uint32_t eflags;
			struct {
				uint8_t cf : 1;
				uint8_t __reserved1 : 1;
				uint8_t pf : 1;
				uint8_t __reserved3 : 1;
				uint8_t af : 1;
				uint8_t __reserved5 : 1;
				uint8_t zf : 1;
				uint8_t sf : 1;
				uint8_t tf : 1;
				uint8_t ief : 1; /* if */
				uint8_t df : 1;
				uint8_t of : 1;
				uint8_t iopl : 1;
				uint8_t nt : 1;
				uint8_t __reserved15 : 1;
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

#endif

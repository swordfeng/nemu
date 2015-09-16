#include <HsFFI.h>
#include <string.h>
#include <stdio.h>
#include "nemu.h"
void init_regex() {
    hs_init(0, 0);
}

uint32_t expr_register_read(const char *name) {
    int index;
    for (index = 0; index < 8; index++) {
	    if (strcmp(name, regsl[index]) == 0) return reg_l(index);
	    if (strcmp(name, regsw[index]) == 0) return reg_w(index);
	    if (strcmp(name, regsb[index]) == 0) return reg_b(index);
	}
	return cpu.eip;
}

uint32_t expr_swaddr_read(swaddr_t addr) {
    return swaddr_read(addr, 4);
}

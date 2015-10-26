#ifndef __INSTR_FETCH_H__
#define __INSTR_FETCH_H__

#include "nemu.h"

inline uint32_t instr_fetch(swaddr_t addr, size_t len) {
	return swaddr_read(addr, len);
}

#endif

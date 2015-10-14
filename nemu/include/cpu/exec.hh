#pragma once

extern "C" {
#include "memory/memory.h"
#include "debug.h"
#include "cpu/instr-fetch.h"
}
#include <cstdio>

extern char assembly[];
#ifdef DEBUG
#define print_asm(...) Assert(snprintf(assembly, 80, __VA_ARGS__) < 80, "buffer overflow!")
#else
#define print_asm(...)
#endif

#define OP_NAMES(name) op_##name##_b, op_##name##_w, op_##name##_l, op_##name##_v
enum OperandName {
    OP_NAMES(suffix), OP_NAMES(rm), OP_NAMES(reg), OP_NAMES(r), OP_NAMES(a), OP_NAMES(moffs), OP_NAMES(imm)
};
#undef OP_NAMES

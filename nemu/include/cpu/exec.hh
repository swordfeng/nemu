#pragma once

extern "C" {
#include "debug.h"
#include "memory/memory.h"
#include "cpu/instr-fetch.h"
}
#include <cstdio>
#include <functional>
#include <type_traits>

extern char assembly[];
#ifdef DEBUG
#define print_asm(...) Assert(snprintf(assembly, 80, __VA_ARGS__) < 80, "buffer overflow!")
#else
#define print_asm(...)
#endif

#define OP_NAMES(name) op_##name##_b, op_##name##_w, op_##name##_l, op_##name##_v
enum OperandName {
    OP_NAMES(rm), OP_NAMES(reg), OP_NAMES(a), OP_NAMES(moffs), OP_NAMES(imm)
};
#undef OP_NAMES

enum OperandType {
    opt_undefined, opt_register, opt_address, opt_immediate
};

struct Operand {
    OperandType type;
    size_t size;
    union {
        int reg_index;
        swaddr_t address;
        uint32_t value;
    };
    Operand();
    template<typename ValueType> ValueType getValue();
    template<typename ValueType> void setValue(ValueType v);
};

template<typename ValueType> ValueType Operand::getValue() {
   Assert(sizeof(ValueType) <= 4 && sizeof(ValueType) == size, "invalid type cast");
   Assert(type != opt_undefined, "operand undefined");
   uint32_t ret = 0;
   switch (type) {
   case opt_register:
       if (size == 1) ret = reg_b(reg_index);
       else if (size == 2) ret = reg_w(reg_index);
       else ret = reg_l(reg_index);
       break;
   case opt_address:
       ret = swaddr_read(address, size);
       break;
   case opt_immediate:
       ret = value;
       break;
   }
   return static_cast<ValueType>(ret);
}

template<typename ValueType> void Operand::setValue(ValueType v) {
    Assert(sizeof(ValueType) == size, "invalid type cast");
    Assert(type != opt_undefined, "operand undefined");
    Assert(type != opt_immediate, "set value to imediate");
    switch (type) {
    case opt_register:
        if (size == 1) reg_b(reg_index) = static_cast<typename std::make_unsigned<ValueType>::type>(v);
        else if (size == 2) reg_w(reg_index) = static_cast<typename std::make_unsigned<ValueType>::type>(v);
        else reg_l(reg_index) = static_cast<typename std::make_unsigned<ValueType>::type>(v);
        break;
    case opt_address:
        swaddr_write(address, size, static_cast<uint32_t>(v));
        break;
    }
}

struct InstructionContext {
    uint32_t prefix[4];
    uint32_t opcode;
    bool require_mod_rm, require_sib;
    size_t disp_len, immediate_len;
    Operand operands[4];
    InstructionContext();
};

#define HELPER_PARAM_LIST (InstructionContext &ctx, swaddr_t eip)

#define HELPER(name) int name HELPER_PARAM_LIST

#define TEMPLATE_HELPER(name) template<OperandName ...operand_names> HELPER(name)

typedef std::function<int (InstructionContext &, swaddr_t)> helper_fun;

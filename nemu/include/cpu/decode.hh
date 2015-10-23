#pragma once

#include <functional>
#include <type_traits>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <cstdio>

extern "C" {
#include "cpu/reg.h"
#include "memory/memory.h"
#include "debug.h"
#include "cpu/instr-fetch.h"
}

using std::string;

/* Set to save operand name */
#define PRINT_INSTR

/* if print instructions, then enable operand output */
#ifdef PRINT_INSTR
#define OPERAND_SET_NAME
#endif

/* Forward Declaration */
struct InstructionContext;

/* print asm */
extern char assembly[];
#ifdef DEBUG
#define print_asm(...) Assert(snprintf(assembly, 80, __VA_ARGS__) < 80, "buffer overflow!")
#else
#define print_asm(...)
#endif
/* print asm automatically */
void print_instr(InstructionContext &ctx, string name);

/* operand names */
#define OP_NAMES(name) op_##name##_b, op_##name##_w, op_##name##_l, op_##name##_v
enum OperandName {
    OP_NAMES(suffix), OP_NAMES(rm), OP_NAMES(reg), OP_NAMES(r), OP_NAMES(moffs),
    OP_NAMES(imm), OP_NAMES(a), OP_NAMES(c),
    op_1_b = 0x80, op_1 = 0x80
};
#undef OP_NAMES

/* Easy way to decide whether a name belongs to a group */
#define op_name_is(opname, groupname) \
    ((opname & ~0x3u) == op_##groupname##_b)

/* operand types */
enum OperandType {
    opt_undefined, opt_register, opt_address, opt_immediate
};

enum InstructionPrefixIndex {
    prefix_lock_rep, prefix_segment, prefix_operand, prefix_address
};

enum InstructionPrefixOpcode {
    prefix_0_lock = 0xf0,
    prefix_0_rep = 0xf3,
    prefix_0_repe = 0xf3,
    prefix_0_repne = 0xf2,
    prefix_2_operand = 0x66,
    prefix_3_address = 0x67
};

/* Operand */
struct Operand {
    OperandType type;
    size_t size;
    union {
        uint8_t reg_index;
        swaddr_t address;
        uint32_t immediate;
    };
#ifdef OPERAND_SET_NAME
    string str_name;
#endif
    Operand();
    inline uint32_t getSignedValue();
    inline uint32_t getUnsignedValue();
    swaddr_t getAddress();
    void setValue(uint32_t v);
    string suffix();
};

/* Instruction Context to be passed through decoding */
struct InstructionContext {
    uint8_t prefix[4];
    uint32_t opcode;
    bool require_modrm;
    Operand operands[4];
    InstructionContext();
    // for templated decode function
    int decoded_len;
    swaddr_t decoded_eip;
};

/* ModR/M */
union ModR_M {
    uint8_t value;
    struct {
        uint8_t rm : 3;
        uint8_t regop : 3;
        uint8_t mod : 2;
    };
};

/* SIB */
union SIB {
    uint8_t value;
    struct {
        uint8_t base : 3;
        uint8_t index : 3;
        uint8_t ss : 2; /* aka scale */
    };
};

/* Helper function parameters */
#define HELPER_PARAM_LIST (InstructionContext &ctx, swaddr_t eip)

/* Normal helper function */
#define HELPER(name) int name HELPER_PARAM_LIST

/* Templated helper function */
#define TEMPLATE_HELPER(name) template<OperandName ...operand_names> HELPER(name)

/**
 * Special helper function for instructions:
 * A templated version to execute decoding operations
 * A normal version to do the real work
 **/
#define TEMPLATE_INSTRUCTION_HELPER(name) inline void name##_internal HELPER_PARAM_LIST; \
    template<OperandName ...operand_names> HELPER(name) { \
        int len = decode_operands<operand_names...>(ctx, eip); \
        cpu.eip += len; \
        name ## _internal(ctx, eip); \
        return len; \
    } \
    inline void name##_internal HELPER_PARAM_LIST

/**
 * Special helper function for instructions
 **/
#define INSTRUCTION_HELPER(name) inline void name##_internal HELPER_PARAM_LIST; \
    inline HELPER(name) { \
        cpu.eip += 1; \
        name##_internal(ctx, eip); \
        return 1; \
    } \
    inline void name##_internal HELPER_PARAM_LIST

/* Functional helper function type */
typedef std::function<int (InstructionContext &, swaddr_t)> helper_fun;

/* Get operand size according to name and instruction prefix */
size_t op_get_size(InstructionContext &ctx, OperandName opname);
/* Extend a signed value (wrapper in uint32_t) */
uint32_t signed_extend(uint32_t val, size_t size);
/* Force trunc a value, make high bits zero */
uint32_t int_trunc(uint32_t val, size_t size);
/* Get SIGNED hex value string */
string conv16(uint32_t val);
/* Fastly calculate PF bit */
uint8_t calc_pf(uint8_t val);

/*** Operands Decoding Function ***/
TEMPLATE_HELPER(decode_operands);
HELPER(decode_operands); // simply check and read result from previous function template

template <size_t size> struct standard_type;
template <> struct standard_type<1> {
    typedef uint8_t type;
};
template <> struct standard_type<2> {
    typedef uint16_t type;
};
template <> struct standard_type<4> {
    typedef uint32_t type;
};

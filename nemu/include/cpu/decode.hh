#pragma once
#include "cpu/exec.hh"
#include <functional>
#include <type_traits>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
using std::string;

enum OperandType {
    opt_undefined, opt_register, opt_address, opt_immediate
};

struct InstructionContext;
struct Operand {
    OperandType type;
    size_t size;
    union {
        uint8_t reg_index;
        swaddr_t address;
        uint32_t immediate;
    };
    string str_name;
    Operand();
    uint32_t getValue();
    uint32_t getSize();
    void setValue(uint32_t v);
};

struct InstructionContext {
    uint8_t prefix[4];
    uint32_t opcode;
    bool require_modrm;
    Operand operands[4];
    InstructionContext();
};

union ModR_M {
    uint8_t value;
    struct {
        uint8_t rm : 3;
        uint8_t regop : 3;
        uint8_t mod : 2;
    };
};

union SIB {
    uint8_t value;
    struct {
        uint8_t base : 3;
        uint8_t index : 3;
        uint8_t ss : 2; /* aka scale */
    };
};

#define HELPER_PARAM_LIST (InstructionContext &ctx, swaddr_t eip)

#define HELPER(name) int name HELPER_PARAM_LIST

#define TEMPLATE_HELPER(name) template<OperandName ...operand_names> HELPER(name)

typedef std::function<int (InstructionContext &, swaddr_t)> helper_fun;

#define op_name_is(opname, groupname) \
    ((opname & ~0x3u) == op_##groupname##_b)

size_t op_get_size(InstructionContext &ctx, OperandName opname);
uint32_t reg_read(int reg_index, size_t size);
string reg_name(int reg_index, size_t size);
uint32_t signed_extend(uint8_t val);
string conv16(uint32_t val); /* Signed!!! */

TEMPLATE_HELPER(decode_operands);

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

inline void print_instr(InstructionContext &ctx, string name) {
    size_t operands_size = 0;
    string showstr = name + "\t";
    while (operands_size < 4 && ctx.operands[operands_size].type != opt_undefined) {
        operands_size++;
    }
    // TODO: prefix
    bool add_comma = false;
    for (int i = static_cast<int>(operands_size) - 1; i >= 0; i--) {
        if (add_comma) showstr += ",";
        showstr += ctx.operands[i].str_name;
        add_comma = true;
    }
    print_asm("%s", showstr.c_str());
}


#define OPERAND_SET_NAME
#include "decode.template.hh"

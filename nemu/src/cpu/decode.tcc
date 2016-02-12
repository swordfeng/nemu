#include <type_traits>
#include "cpu/decode.hh"

inline size_t op_get_size(InstructionContext &ctx, OperandName opname) {
    switch (opname & 0x3u) {
    case op_suffix_b:
        return 1;
    case op_suffix_w:
        return 2;
    case op_suffix_l:
        return 4;
    case op_suffix_v:
        return ctx.prefix[2] == 0 ? 4 : 2;
    }
    return 0;
}

static inline const char *prefix_name(int opcode, int prefix_code) {
    switch (prefix_code) {
    case prefix_0_rep: /* == repe */
        return ((opcode | ~1) == 0xa6) || ((opcode | ~1) == 0xae) ? "repe " : "rep ";
    case prefix_0_repne:
        return "repne ";
    case prefix_0_lock:
        return "lock ";
    default:
        return "";
    }
}

#ifdef PRINT_INSTR
inline void print_instr(InstructionContext &ctx, string name) {
    size_t operands_size = 0;
    while (operands_size < 4 && ctx.operands[operands_size].type != opt_undefined) {
        operands_size++;
    }
    string showstr;
    showstr.reserve(24);
    showstr = prefix_name(ctx.opcode, ctx.prefix[0]);
    showstr += name;
    bool print_suffix = operands_size != 0;
    for (size_t i = 0; i < operands_size; i++) {
        if (ctx.operands[i].type == opt_register) {
            print_suffix = false;
            break;
        }
    }
    if (operands_size > 0 && print_suffix) showstr += ctx.operands[0].suffix();
    showstr += "\t";
    bool add_comma = false;
    for (int i = static_cast<int>(operands_size) - 1; i >= 0; i--) {
        if (add_comma) showstr += ",";
        showstr += ctx.operands[i].str_name;
        add_comma = true;
    }
    Assert(showstr.size() < 80, "assembly buffer overflow");
    std::copy(showstr.begin(), showstr.end(), assembly);
    assembly[showstr.size()] = 0;
}
#else
#define print_instr(...);
#endif

inline InstructionContext::InstructionContext():
opcode(0), require_modrm(false), decoded_len(0) {
    memset(prefix, 0, 4 * sizeof(prefix[0]));
}

template <typename int_type>
inline int_type signed_extend(uint64_t val, size_t size) {
    switch (size) {
    case 1:
        return static_cast<typename std::make_signed<int_type>::type>(static_cast<int8_t>(val));
    case 2:
        return static_cast<typename std::make_signed<int_type>::type>(static_cast<int16_t>(val));
    case 4:
        return static_cast<typename std::make_signed<int_type>::type>(static_cast<int32_t>(val));
    case 8:
        return static_cast<typename std::make_signed<int_type>::type>(static_cast<int64_t>(val));
    default:
        panic("invalid data size");
    }
}

inline uint32_t signed_extend(uint32_t val, size_t size) {
    return signed_extend<uint32_t>(val, size);
}

inline uint32_t int_trunc(uint32_t val, size_t size) {
    return val & ~(~0u << (size * 4) << (size * 4));
}

inline string conv16(uint32_t val) {
    static char conv_buf[16];
    if (static_cast<int32_t>(val) < 0) {
        snprintf(conv_buf, 16, "-%#x", -val);
    } else {
        snprintf(conv_buf, 16, "%#x", val);
    }
    return conv_buf;
}

inline uint8_t calc_pf(uint8_t val) {
    val ^= val >> 1;
    val ^= val >> 2;
    val ^= val >> 4;
    return !(val & 1);
}

#define DECODE_TEMPLATE_HELPER_OPS(name, n) \
    template <size_t index, OperandName ...operand_names> struct name; \
    template <size_t index> struct name<index> { \
        static inline HELPER(call) { return 0; } \
    }; \
    template <size_t index, OperandName opname, OperandName opname2, OperandName ...operand_names> \
    struct name<index, opname, opname2, operand_names...> { \
        static inline HELPER(call) { \
            int ret1 = name<index, opname>::call(ctx, eip); \
            int ret2 = name<index + n, opname2, operand_names...>::call(ctx, eip); \
            return ret2 > ret1 ? ret2 : ret1; \
        } \
    }; \
    template <size_t index, OperandName opname> \
    struct name<index, opname> { \
        static inline HELPER(call); \
    }; \
    template <size_t index, OperandName opname> \
    inline int name<index, opname>::call HELPER_PARAM_LIST

#define DECODE_TEMPLATE_HELPER(name) DECODE_TEMPLATE_HELPER_OPS(name, 1)

/* decode modrm, sib, disp */
DECODE_TEMPLATE_HELPER(decode_modrm_disp) {
    if (op_name_is(opname, reg)) {
        ctx.require_modrm = true;
        ModR_M modrm;
        modrm.value = instr_fetch(eip, 1);
        ctx.operands[index].type = opt_register;
        ctx.operands[index].reg_index = modrm.regop;
        ctx.operands[index].size = op_get_size(ctx, opname);
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("%") +
            reg_get_name(ctx.operands[index].reg_index, ctx.operands[index].size);
#endif
        return 1;
    } else if (opname == op_reg_cr) {
        ctx.require_modrm = true;
        ModR_M modrm;
        modrm.value = instr_fetch(eip, 1);
        ctx.operands[index].type = opt_register_cr;
        ctx.operands[index].reg_index = modrm.regop;
        if (modrm.regop == 1 || (modrm.regop != 8 && modrm.regop > 4)) {
            panic("Invalid CR register");
        }
        ctx.operands[index].size = 4;
#ifdef OPERAND_SET_NAME
        static char num[] = "0";
        num[0] += ctx.operands[index].reg_index;
        ctx.operands[index].str_name = string("%cr") + num;
        num[0] -= ctx.operands[index].reg_index;
#endif
        return 1;
    } else if (opname == op_reg_seg) {
        ctx.require_modrm = true;
        ModR_M modrm;
        modrm.value = instr_fetch(eip, 1);
        ctx.operands[index].type = opt_register_seg;
        ctx.operands[index].reg_index = modrm.regop;
        if (modrm.regop > 5) {
            panic("Invalid segment register");
        }
        ctx.operands[index].size = 2;
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("%") + reg_seg_get_name(modrm.regop);
#endif
        return 1;
    } else if (op_name_is(opname, rm)) {
        Assert(ctx.prefix[3] == 0, "Address size prefix not implemented");
        ctx.require_modrm = true;
        int ret = 0;
        ModR_M modrm;
        modrm.value = instr_fetch(eip, 1);
        if (modrm.mod == 0x3) {
            // register
            ctx.operands[index].type = opt_register;
            ctx.operands[index].reg_index = modrm.rm;
            ctx.operands[index].size = op_get_size(ctx, opname);
#ifdef OPERAND_SET_NAME
            ctx.operands[index].str_name = string("%") +
                reg_get_name(ctx.operands[index].reg_index, ctx.operands[index].size);
#endif
            ret = 1;
        } else {
            if (modrm.rm == 4) {
                //ctx.require_sib = true;
                // sib...
                ret = 2;
                SIB sib;
                sib.value = instr_fetch(eip + 1, 1);
                ctx.operands[index].type = opt_address;
                ctx.operands[index].sreg = sreg_index(ds);
#ifdef OPERAND_SET_NAME
                string base_name, index_name;
#endif
                uint32_t addr = 0, base_addr = 0;
                uint8_t scale_factor = (1u << sib.ss);
                /* index */
                if (sib.index != 4) {
#ifdef OPERAND_SET_NAME
                    index_name = string("%") + reg_get_name(sib.index, 4);
#endif
                    addr = reg_read_index(sib.index, 4);
                }
                addr *= scale_factor;
                /* base */
                if (sib.base != 5 || modrm.mod != 0) {
#ifdef OPERAND_SET_NAME
                    base_name = string("%") + reg_get_name(sib.base, 4);
#endif
                    base_addr = reg_read_index(sib.base, 4);
                } else {
                    // manually fetch disp32
                    base_addr = instr_fetch(eip + 2, 4);
                    ret += 4;
                }
                ctx.operands[index].address = addr + base_addr;
                ctx.operands[index].size = op_get_size(ctx, opname);
                if (sib.index == 5) {
                    // BP in effective address
                    ctx.operands[index].sreg = sreg_index(ss);
                }
#ifdef OPERAND_SET_NAME
                char scale_factor_str[] = "x";
                scale_factor_str[0] = scale_factor + '0';
                ctx.operands[index].str_name = "(" + base_name + "," + index_name + "," + scale_factor_str + ")";
                if (sib.base == 5 && modrm.rm == 0) {
                    ctx.operands[index].str_name = conv16(base_addr) + ctx.operands[index].str_name;
                }
#endif
            } else if (modrm.mod == 0 && modrm.rm == 5) {
                // disp32
                //ctx.disp_size = 4;
                uint32_t disp = instr_fetch(eip + 1, 4);
                ctx.operands[index].type = opt_address;
                ctx.operands[index].address = disp;
                ctx.operands[index].sreg = sreg_index(ds);
                ctx.operands[index].size = op_get_size(ctx, opname);
#ifdef OPERAND_SET_NAME
                ctx.operands[index].str_name = conv16(disp);
#endif
                ret = 5;
            } else {
                // [EAX]...
                ctx.operands[index].type = opt_address;
                ctx.operands[index].size = op_get_size(ctx, opname);
                ctx.operands[index].address = reg_read_index(modrm.rm, 4);
                ctx.operands[index].sreg = sreg_index(ds);
                if (modrm.rm == 2 || modrm.rm == 3 || modrm.rm == 6) {
                    // BP in effective address
                    ctx.operands[index].sreg = sreg_index(ss);
                }
#ifdef OPERAND_SET_NAME
                ctx.operands[index].str_name = string("(%") +
                    reg_get_name(modrm.rm, 4) + ")";
#endif

                ret = 1;
            }
            if (modrm.mod == 1) {
                // +disp8
                //ctx.disp_size = 1;
                uint32_t disp = signed_extend(instr_fetch(eip + ret, 1), 1);
                ctx.operands[index].address += disp;
#ifdef OPERAND_SET_NAME
                ctx.operands[index].str_name = conv16(disp) + ctx.operands[index].str_name;
#endif
                ret += 1;
            } else if (modrm.mod == 2) {
                // +disp32
                //ctx.disp_size = 4;
                uint32_t disp = instr_fetch(eip + ret, 4);
                ctx.operands[index].address += disp;
#ifdef OPERAND_SET_NAME
                ctx.operands[index].str_name = conv16(disp) + ctx.operands[index].str_name;
#endif
                ret += 4;
            }
        }
        return ret;
    } else {
        if (ctx.require_modrm) return 1;
        else return 0;
    }
}

DECODE_TEMPLATE_HELPER(decode_imm) {
    if (op_name_is(opname, imm)) {
        ctx.operands[index].type = opt_immediate;
        ctx.operands[index].immediate = instr_fetch(eip, op_get_size(ctx, opname));
        ctx.operands[index].size = op_get_size(ctx, opname);
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("$") + conv16(ctx.operands[index].immediate);
#endif
        return op_get_size(ctx, opname);
    } else return 0;
}

DECODE_TEMPLATE_HELPER(decode_moffs) {
    if (op_name_is(opname, moffs)) {
        ctx.operands[index].type = opt_address;
        ctx.operands[index].address = instr_fetch(eip, 4);
        ctx.operands[index].sreg = sreg_index(ds);
        ctx.operands[index].size = op_get_size(ctx, opname);
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("$") + conv16(ctx.operands[index].immediate);
#endif
        return 4;
    } else return 0;
}

DECODE_TEMPLATE_HELPER_OPS(decode_ptrwv, 2) {
    if (opname == op_ptrwv) {
        size_t offsize = ctx.prefix[2] == 0 ? 4 : 2;
        ctx.operands[index].type = opt_immediate;
        ctx.operands[index].immediate = instr_fetch(eip, offsize);
        ctx.operands[index].size = offsize;
        ctx.operands[index + 1].type = opt_immediate;
        ctx.operands[index + 1].immediate = instr_fetch(eip + offsize, 2);
        ctx.operands[index + 1].size = 2;
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("$") + conv16(ctx.operands[index].immediate);
        ctx.operands[index + 1].str_name = string("$") + conv16(ctx.operands[index + 1].immediate);
#endif
        return offsize + 2;
    } else return 0;
}
/*
DECODE_TEMPLATE_HELPER_OPS(decode_m16v, 2) {
    if (opname == op_m16v) {
        size_t offsize = ctx.prefix[2] == 0 ? 4 : 2;
        ctx.operands[index].type = opt_address;
        ctx.operands[index].value = instr_fetch(eip, offsize);
        ctx.operands[index].size = offsize;
        ctx.operands[index + 1].type = opt_immediate;
        ctx.operands[index + 1].value = instr_fetch(eip + offsize, 2);
        ctx.operands[index + 1].size = 2;
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("$") + conv16(ctx.operands[index].immediate);
        ctx.operands[index + 1].str_name = string("$") + conv16(ctx.operands[index + 1].immediate);
#endif
        return offsize + 2;
    } else return 0;
}
*/
// decode type 'a', 'c', '1' ...
DECODE_TEMPLATE_HELPER(decode_const) {
    if (op_name_is(opname, a)) {
        ctx.operands[index].type = opt_register;
        ctx.operands[index].reg_index = R_EAX;
        ctx.operands[index].size = op_get_size(ctx, opname);
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("%") + reg_get_name(R_EAX, op_get_size(ctx, opname));
#endif
    } else if (op_name_is(opname, c)) {
        ctx.operands[index].type = opt_register;
        ctx.operands[index].reg_index = R_ECX;
        ctx.operands[index].size = op_get_size(ctx, opname);
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("%") + reg_get_name(R_ECX, op_get_size(ctx, opname));
#endif
    } else if (op_name_is(opname, 1)) {
        ctx.operands[index].type = opt_immediate;
        ctx.operands[index].immediate = 1;
        ctx.operands[index].size = 4;
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("$1");
#endif
    }
    return 0;
}

DECODE_TEMPLATE_HELPER(decode_r) {
    if (op_name_is(opname, r)) {
        ctx.operands[index].type = opt_register;
        ctx.operands[index].reg_index = instr_fetch(eip, 1) & 0x7;
        ctx.operands[index].size = op_get_size(ctx, opname);
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("%") + reg_get_name(ctx.operands[index].reg_index, op_get_size(ctx, opname));
#endif
    }
    return 0;
}

#undef DECODE_TEMPLATE_HELPER

TEMPLATE_HELPER(decode_operands) {
    Assert(ctx.prefix[3] == 0, "Address prefix is not implemented");
    decode_r<0, operand_names...>::call(ctx, eip);
    int consumed_size = 1;
    decode_const<0, operand_names...>::call(ctx, eip + consumed_size);
    int modrm_size = decode_modrm_disp<0, operand_names...>::call(ctx, eip + consumed_size);
    consumed_size += modrm_size;
    int moffs_size = decode_moffs<0, operand_names...>::call(ctx, eip + consumed_size);
    consumed_size += moffs_size;
    int imm_size = decode_imm<0, operand_names...>::call(ctx, eip + consumed_size);
    consumed_size += imm_size;
    int ptrwv_size = decode_ptrwv<0, operand_names...>::call(ctx, eip + consumed_size);
    consumed_size += ptrwv_size;
    //printf("opcode = %x, decoded length = %d, m = %d, o = %d, i = %d\n", instr_fetch(eip, 1), consumed_size, modrm_size, moffs_size, imm_size);
    ctx.decoded_len = consumed_size;
    ctx.decoded_eip = eip;
    return consumed_size;
}

inline HELPER(decode_operands) {
    Assert(eip == ctx.decoded_eip, "unexpected call");
    Assert(ctx.decoded_len > 0, "unexpected call");
    return ctx.decoded_len;
}

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

static inline uint32_t reg_read_index(uint8_t reg_index, size_t size) {
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

inline static string reg_get_name(int reg_index, size_t size) {
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

inline void print_instr(InstructionContext &ctx, string name) {
    size_t operands_size = 0;
    while (operands_size < 4 && ctx.operands[operands_size].type != opt_undefined) {
        operands_size++;
    }
    // TODO: prefix
    string showstr = name;
    bool print_suffix = true;
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
    print_asm("%s", showstr.c_str());
}

inline InstructionContext::InstructionContext():
opcode(0), require_modrm(false), decoded_len(0) {
    memset(prefix, 0, 4 * sizeof(prefix[0]));
}

inline Operand::Operand(): type(opt_undefined) {}

inline uint32_t Operand::getSignedValue() {
   uint32_t ret = 0;
   switch (type) {
   case opt_undefined:
       panic("operand undefined");
   case opt_register:
       ret = reg_read_index(reg_index, size);
       break;
   case opt_address:
       ret = swaddr_read(address, size);
       break;
   case opt_immediate:
       ret = immediate;
       break;
   }
   return signed_extend(ret, size);
}

inline uint32_t Operand::getUnsignedValue() {
   uint32_t ret = 0;
   switch (type) {
   case opt_undefined:
       panic("operand undefined");
   case opt_register:
       ret = reg_read_index(reg_index, size);
       break;
   case opt_address:
       ret = swaddr_read(address, size);
       break;
   case opt_immediate:
       ret = immediate;
       break;
   }
   return ret;
}

inline swaddr_t Operand::getAddress() {
    Assert(type == opt_address, "operand is not from memory address");
    return address;
}

inline void Operand::setValue(uint32_t v) {
    switch (type) {
    case opt_undefined:
        panic("operand undefined");
    case opt_immediate:
        panic("set value to imediate");
    case opt_register:
        if (size == 1) reg_b(reg_index) = static_cast<uint8_t>(v);
        else if (size == 2) reg_w(reg_index) = static_cast<uint16_t>(v);
        else reg_l(reg_index) = v;
        break;
    case opt_address:
        swaddr_write(address, size, v);
        break;
    }
}

inline string Operand::suffix() {
    switch (size) {
    case 1:
        return "b";
    case 2:
        return "w";
    case 4:
        return "l";
    }
    panic("wrong size");
    return "";
}

inline uint32_t signed_extend(uint32_t val, size_t size) {
    switch (size) {
    case 1:
        return static_cast<uint32_t>(static_cast<int8_t>(val));
    case 2:
        return static_cast<uint32_t>(static_cast<int16_t>(val));
    case 4:
        return val;
    default:
        panic("invalid data size");
    }
}

inline uint32_t int_trunc(uint32_t val, size_t size) {
    return val & ~(~0u << (size * 4) << (size * 4));
}

inline string conv16(uint32_t val) {
    std::stringstream ss;
    if (static_cast<int32_t>(val) < 0) {
        ss << "-";
        val = -static_cast<int32_t>(val);
    }
    ss << "0x" << std::hex << val;
    return ss.str();
}

inline uint8_t calc_pf(uint8_t val) {
    val ^= val >> 1;
    val ^= val >> 2;
    val ^= val >> 4;
    return val & 1;
}

#define DECODE_TEMPLATE_HELPER(name) \
    template <size_t index, OperandName ...operand_names> struct name; \
    template <size_t index> struct name<index> { \
        static HELPER(call) { return 0; } \
    }; \
    template <size_t index, OperandName opname, OperandName opname2, OperandName ...operand_names> \
    struct name<index, opname, opname2, operand_names...> { \
        static HELPER(call) { \
            int ret1 = name<index, opname>::call(ctx, eip); \
            int ret2 = name<index + 1, opname2, operand_names...>::call(ctx, eip); \
            return ret2 > ret1 ? ret2 : ret1; \
        } \
    }; \
    template <size_t index, OperandName opname> \
    struct name<index, opname> { \
        static HELPER(call); \
    }; \
    template <size_t index, OperandName opname> \
    int name<index, opname>::call HELPER_PARAM_LIST

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
                string base_name, index_name;
                uint32_t addr = 0, base_addr = 0;
                uint8_t scale_factor = (1u << sib.ss);
                /* index */
                if (sib.index != 4) {
                    index_name = string("%") + reg_get_name(sib.index, 4);
                    addr = reg_read_index(sib.index, 4);
                }
                addr *= scale_factor;
                /* base */
                if (sib.base != 5 || modrm.rm != 0) {
                    base_name = string("%") + reg_get_name(sib.base, 4);
                    base_addr = reg_read_index(sib.base, 4);
                } else {
                    // manually fetch disp32
                    base_addr = instr_fetch(eip + 2, 4);
                    ret += 4;
                }
                ctx.operands[index].address = addr + base_addr;
                ctx.operands[index].size = op_get_size(ctx, opname);
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
        ctx.operands[index].size = op_get_size(ctx, opname);
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("$") + conv16(ctx.operands[index].immediate);
#endif
        return 4;
    } else return 0;
}

DECODE_TEMPLATE_HELPER(decode_a) {
    if (op_name_is(opname, a)) {
        ctx.operands[index].type = opt_register;
        ctx.operands[index].reg_index = R_EAX;
        ctx.operands[index].size = op_get_size(ctx, opname);
#ifdef OPERAND_SET_NAME
        ctx.operands[index].str_name = string("%") + reg_get_name(R_EAX, op_get_size(ctx, opname));
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
    decode_a<0, operand_names...>::call(ctx, eip + consumed_size);
    int modrm_size = decode_modrm_disp<0, operand_names...>::call(ctx, eip + consumed_size);
    consumed_size += modrm_size;
    int moffs_size = decode_moffs<0, operand_names...>::call(ctx, eip + consumed_size);
    consumed_size += moffs_size;
    int imm_size = decode_imm<0, operand_names...>::call(ctx, eip + consumed_size);
    consumed_size += imm_size;
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

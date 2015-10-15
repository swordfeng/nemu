template <OperandName op> HELPER(jcxz) {
    int len = decode_operands<op>(ctx, eip);
    uint32_t val;
    string instr_name;
    if (ctx.prefix[3]) {
        // address prefix
        val = reg_b(R_CX);
        instr_name = "jcxz";
    } else {
        val = reg_l(R_ECX);
        instr_name = "jecxz";
    }
    print_asm("%s %x", instr_name.c_str(), eip + len + ctx.operands[0].getSignedValue());
    if (val == 0) return len + ctx.operands[0].getSignedValue();
    else return len;
}


#define JCC_HELPER(name, cond) \
template <OperandName op> HELPER(j##name) { \
    int len = decode_operands<op>(ctx, eip); \
    swaddr_t temp_eip = eip + len + ctx.operands[0].getSignedValue(); \
    if (ctx.operands[0].size == 4) temp_eip = int_trunc(temp_eip, 4); \
    print_asm("%s %x", "j" #name, temp_eip); \
    if (cond) return temp_eip - eip; \
    else return len; \
}

JCC_HELPER(o, cpu.of)
JCC_HELPER(no, !cpu.of)
JCC_HELPER(b, cpu.cf)
JCC_HELPER(ae, !cpu.cf)
JCC_HELPER(e, cpu.zf)
JCC_HELPER(ne, !cpu.zf)
JCC_HELPER(be, cpu.cf || cpu.zf)
JCC_HELPER(a, !cpu.cf && !cpu.zf)
JCC_HELPER(s, cpu.sf);
JCC_HELPER(ns, !cpu.sf);
JCC_HELPER(p, cpu.pf);
JCC_HELPER(np, !cpu.pf);
JCC_HELPER(l, cpu.sf != cpu.of)
JCC_HELPER(ge, cpu.sf == cpu.of)
JCC_HELPER(le, cpu.zf || cpu.sf != cpu.of)
JCC_HELPER(g, !cpu.zf && cpu.sf == cpu.of)

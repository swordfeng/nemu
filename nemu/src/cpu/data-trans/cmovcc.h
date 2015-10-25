#define CMOVCC_HELPER(name, cond) \
TEMPLATE_INSTRUCTION_HELPER(cmov##name) { \
    if (!cond) return; \
    ctx.operands[0].setValue(ctx.operands[1].getUnsignedValue()); \
    print_instr(ctx, "cmov" #name); \
}

CMOVCC_HELPER(o, cpu.of)
CMOVCC_HELPER(no, !cpu.of)
CMOVCC_HELPER(b, cpu.cf)
CMOVCC_HELPER(ae, !cpu.cf)
CMOVCC_HELPER(e, cpu.zf)
CMOVCC_HELPER(ne, !cpu.zf)
CMOVCC_HELPER(be, cpu.cf || cpu.zf)
CMOVCC_HELPER(a, !cpu.cf && !cpu.zf)
CMOVCC_HELPER(s, cpu.sf);
CMOVCC_HELPER(ns, !cpu.sf);
CMOVCC_HELPER(p, cpu.pf);
CMOVCC_HELPER(np, !cpu.pf);
CMOVCC_HELPER(l, cpu.sf != cpu.of)
CMOVCC_HELPER(ge, cpu.sf == cpu.of)
CMOVCC_HELPER(le, cpu.zf || cpu.sf != cpu.of)
CMOVCC_HELPER(g, !cpu.zf && cpu.sf == cpu.of)

#undef CMOVCC_HELPER

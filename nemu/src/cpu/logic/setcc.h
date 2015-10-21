#define SETCC_HELPER(name, cond) \
TEMPLATE_INSTRUCTION_HELPER(set##name) { \
    print_instr(ctx, "set" #name); \
    ctx.operands[0].setValue(cond ? 1 : 0); \
}

SETCC_HELPER(o, cpu.of)
SETCC_HELPER(no, !cpu.of)
SETCC_HELPER(b, cpu.cf)
SETCC_HELPER(ae, !cpu.cf)
SETCC_HELPER(e, cpu.zf)
SETCC_HELPER(ne, !cpu.zf)
SETCC_HELPER(be, cpu.cf || cpu.zf)
SETCC_HELPER(a, !cpu.cf && !cpu.zf)
SETCC_HELPER(s, cpu.sf);
SETCC_HELPER(ns, !cpu.sf);
SETCC_HELPER(p, cpu.pf);
SETCC_HELPER(np, !cpu.pf);
SETCC_HELPER(l, cpu.sf != cpu.of)
SETCC_HELPER(ge, cpu.sf == cpu.of)
SETCC_HELPER(le, cpu.zf || cpu.sf != cpu.of)
SETCC_HELPER(g, !cpu.zf && cpu.sf == cpu.of)

#undef SETCC_HELPER

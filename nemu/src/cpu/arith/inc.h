TEMPLATE_INSTRUCTION_HELPER(inc) {
    uint32_t src = ctx.operands[0].getSignedValue();
    uint32_t result = src + 1;
    ctx.operands[0].setValue(result);
    cpu.of = ((src >> 31) == 0) && ((ctx.operands[0].getSignedValue() >> 31) == 1);
    cpu.sf = ctx.operands[0].getSignedValue() >> 31;
    cpu.zf = result == 0;
    cpu.pf = calc_pf(result);
    print_instr(ctx, "inc");
}

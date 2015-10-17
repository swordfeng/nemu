TEMPLATE_INSTRUCTION_HELPER(neg) {
    int len = decode_operands(ctx, eip);
    uint32_t src = ctx.operands[0].getSignedValue();
    uint32_t result = ~src + 1;
    ctx.operands[0].setValue(result);
    cpu.cf = src == 0;
    cpu.of = ((src >> 31) == (ctx.operands[0].getSignedValue() >> 31));
    cpu.sf = ctx.operands[0].getSignedValue() >> 31;
    cpu.zf = result == 0;
    cpu.pf = calc_pf(result);
    print_instr(ctx, "neg");
    return len;
}

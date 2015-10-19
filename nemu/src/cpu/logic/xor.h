TEMPLATE_INSTRUCTION_HELPER(xor_) {
    uint32_t result = ctx.operands[0].getSignedValue() ^ ctx.operands[1].getSignedValue();
    ctx.operands[0].setValue(result);
    cpu.sf = result >> 31;
    cpu.zf = result == 0;
    cpu.pf = calc_pf(result);
    cpu.cf = 0;
    cpu.of = 0;
    print_instr(ctx, "test");
}

TEMPLATE_INSTRUCTION_HELPER(test) {
    uint32_t test_res = ctx.operands[0].getSignedValue() & ctx.operands[1].getSignedValue();
    cpu.sf = test_res >> 31;
    cpu.zf = test_res == 0;
    cpu.pf = calc_pf(test_res);
    cpu.cf = 0;
    cpu.of = 0;
    print_instr(ctx, "test");
}

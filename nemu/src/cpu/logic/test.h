TEMPLATE_INSTRUCTION_HELPER(test) {
    int len = decode_operands(ctx, eip);
    uint32_t test_res = ctx.operands[0].getSignedValue() & ctx.operands[1].getSignedValue();
    cpu.sf = test_res >> 31;
    cpu.zf = test_res == 0;
    cpu.pf = calc_pf(test_res);
    cpu.cf = 0;
    cpu.of = 0;
    string instr_name = "test";
    if (ctx.operands[0].type != opt_register && ctx.operands[1].type != opt_register) {
        instr_name += ctx.operands[0].suffix();
    }
    print_instr(ctx, instr_name);
    return len;
}

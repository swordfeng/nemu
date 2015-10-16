TEMPLATE_INSTRUCTION_HELPER(sub) {
    int len = decode_operands(ctx, eip);
    uint32_t val1 = ctx.operands[0].getSignedValue();
    uint32_t val2 = ctx.operands[1].getSignedValue();
    size_t result_size = ctx.operands[0].size;
    uint32_t result = signed_extend(int_trunc(val1 - val2, result_size), result_size);

    cpu.cf = int_trunc(result, result_size) > int_trunc(val1, result_size);
    cpu.zf = result == 0;
    cpu.sf = result >> 31;
    cpu.of = (val1 >> 31) != (val2 >> 31) && (val1 >> 31) != (result >> 31);
    cpu.pf = calc_pf(result);
    // AF ignored
    string instr_name = "cmp";
    if (ctx.operands[0].type != opt_register && ctx.operands[1].type != opt_register) {
        instr_name += ctx.operands[0].suffix();
    }
    print_instr(ctx, instr_name);
    return len;
}

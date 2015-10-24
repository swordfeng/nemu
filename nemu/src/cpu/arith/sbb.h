TEMPLATE_INSTRUCTION_HELPER(sbb) {
    uint32_t val1 = ctx.operands[0].getSignedValue();
    uint32_t val2 = ctx.operands[1].getSignedValue();
    size_t result_size = ctx.operands[0].size;
    uint32_t result = signed_extend(val1 - (val2 + cpu.cf), result_size);
    ctx.operands[0].setValue(result);
    cpu.cf = result > val1;
    cpu.zf = result == 0;
    cpu.sf = result >> 31;
    cpu.of = (val1 >> 31) != (val2 >> 31) && (val1 >> 31) != (result >> 31);
    cpu.pf = calc_pf(result);
    // AF ignored
    print_instr(ctx, "sbb");
}

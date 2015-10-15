TEMPLATE_INSTRUCTION_HELPER(mov) {
    int len = decode_operands(ctx, eip);
    ctx.operands[0].setValue(ctx.operands[1].getUnsignedValue());
    string instr_name = "mov";
    if (ctx.operands[0].type == opt_address &&
        (ctx.operands[1].type == opt_address || ctx.operands[1].type == opt_immediate)) {
        instr_name += ctx.operands[0].suffix();
    }
    print_instr(ctx, instr_name);
    return len;
}

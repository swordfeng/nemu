TEMPLATE_INSTRUCTION_HELPER(xchg) {
    int len = decode_operands(ctx, eip);
    uint32_t t = ctx.operands[0].getUnsignedValue();
    ctx.operands[0].setValue(ctx.operands[1].getUnsignedValue());
    ctx.operands[1].setValue(t);
    print_instr(ctx, "xchg");
    return len;
}

TEMPLATE_INSTRUCTION_HELPER(mov) {
    int len = decode_operands(ctx, eip);
    ctx.operands[0].setValue(ctx.operands[1].getUnsignedValue());
    print_instr(ctx, "mov");
    return len;
}

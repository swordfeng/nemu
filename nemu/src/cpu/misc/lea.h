TEMPLATE_INSTRUCTION_HELPER(lea) {
    int len = decode_operands(ctx, eip);
    ctx.operands[0].setValue(ctx.operands[1].getAddress());
    return len;
}

TEMPLATE_INSTRUCTION_HELPER(movzx) {
    ctx.operands[0].setValue(ctx.operands[1].getUnsignedValue());
    print_instr(ctx, "movz" + ctx.operands[1].suffix() + ctx.operands[0].suffix());
}

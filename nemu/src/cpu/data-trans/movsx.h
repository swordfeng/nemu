TEMPLATE_INSTRUCTION_HELPER(movsx) {
    ctx.operands[0].setValue(ctx.operands[1].getSignedValue());
    print_instr(ctx, "movs" + ctx.operands[1].suffix() + ctx.operands[0].suffix());
}

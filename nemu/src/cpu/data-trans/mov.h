TEMPLATE_INSTRUCTION_HELPER(mov) {
    ctx.operands[0].setValue(ctx.operands[1].getUnsignedValue());
    print_instr(ctx, "mov");
}

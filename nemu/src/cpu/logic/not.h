TEMPLATE_INSTRUCTION_HELPER(not_) {
    ctx.operands[0].setValue(~ctx.operands[0].getUnsignedValue());
    print_instr(ctx, "not");
}

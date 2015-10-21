TEMPLATE_INSTRUCTION_HELPER(lea) {
    ctx.operands[0].setValue(ctx.operands[1].getAddress());
    print_instr(ctx, "lea");
}

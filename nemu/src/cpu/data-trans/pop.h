TEMPLATE_INSTRUCTION_HELPER(pop) {
    // NOT IMPLEMENTED: stack address size detection
    ctx.operands[0].setValue(swaddr_read(reg_l(R_ESP), ctx.operands[0].size));
    reg_l(R_ESP) += ctx.operands[0].size;
    print_instr(ctx, "pop");
}

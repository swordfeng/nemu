TEMPLATE_INSTRUCTION_HELPER(pop) {
    // NOT IMPLEMENTED: stack address size detection
    int len = decode_operands(ctx, eip);
    ctx.operands[0].setValue(swaddr_read(reg_l(R_ESP), ctx.operands[0].size));
    reg_l(R_ESP) += ctx.operands[0].size;
    return len;
}

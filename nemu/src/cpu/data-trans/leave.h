HELPER(leave) {
    // NOT IMPLEMENTED: stack address size detection
    reg_l(R_ESP) = reg_l(R_EBP);
    reg_w(R_EBP) = swaddr_read(reg_l(R_ESP), ctx.prefix[prefix_operand] ? 2 : 4);
    reg_l(R_ESP) += ctx.prefix[2] ? 2 : 4;
    return 1;
}

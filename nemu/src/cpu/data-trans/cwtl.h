INSTRUCTION_HELPER(cwtl) {
    if (ctx.prefix[prefix_operand]) {
        // AX <- AL
        reg_w(R_AX) = signed_extend(reg_b(R_AL), 1);
        print_asm("cbtw");
    } else {
        // EAX <- AX
        reg_l(R_EAX) = signed_extend(reg_w(R_AX), 2);
        print_asm("cwtl");
    }
}

INSTRUCTION_HELPER(cwtl_cltd) {
    if (ctx.prefix[prefix_operand]) {
        // DX:AX <- AX
        reg_w(R_DX) = signed_extend(reg_w(R_AX), 2) >> 16;
        print_asm("cwtl");
    } else {
        // EDX:EAX <- EAX
        reg_l(R_EDX) = signed_extend<int64_t>(reg_l(R_EAX), 4) >> 32;
        print_asm("cltd");
    }
}

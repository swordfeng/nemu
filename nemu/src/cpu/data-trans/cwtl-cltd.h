INSTRUCTION_HELPER(cwtl_cltd) {
    if (ctx.prefix[prefix_operand]) {
        // DX:AX <- AX
        reg_w(R_DX) = signed_extend(reg_w(R_AX), 4) >> 16;
        print_asm("cwtl");
    } else {
        // EDX:EAX <- EAX
        printf("before %x\n", reg_l(R_EDX));
        reg_l(R_EDX) = static_cast<int32_t>(reg_l(R_EAX)) >> 16;
        printf("after %x\n", reg_l(R_EDX));
        print_asm("cltd");
    }
}

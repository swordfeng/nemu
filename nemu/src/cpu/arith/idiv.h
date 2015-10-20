TEMPLATE_INSTRUCTION_HELPER(idiv) {
    int32_t val = ctx.operands[0].getSignedValue();
    Assert(val != 0, "devide by zero");
    int64_t src = 0;
    switch (ctx.operands[0].size) {
    case 1:
        src = static_cast<int16_t>(reg_w(R_AX));
        break;
    case 2:
        src = static_cast<int16_t>(reg_w(R_DX));
        src <<= 16;
        src |= reg_w(R_AX);
        break;
    case 4:
        src = static_cast<int32_t>(reg_w(R_EDX));
        src <<= 32;
        src |= reg_w(R_AX);
    }
    int32_t quotient = src / val;
    int32_t remainder = src % val;
    switch (ctx.operands[0].size) {
    case 1:
        reg_b(R_AL) = quotient & 0xff;
        reg_b(R_AH) = remainder & 0xff;
        break;
    case 2:
        reg_w(R_AX) = quotient & 0xffff;
        reg_w(R_DX) = remainder & 0xffff;
        break;
    case 4:
        reg_l(R_EAX) = quotient;
        reg_l(R_EDX) = remainder;
    }
    print_instr(ctx, "idiv");
}

TEMPLATE_INSTRUCTION_HELPER(div) {
    uint32_t val = ctx.operands[0].getUnsignedValue();
    Assert(val != 0, "devide by zero");
    uint64_t src = 0;
    switch (ctx.operands[0].size) {
    case 1:
        src = reg_w(R_AX);
        break;
    case 2:
        src = (reg_w(R_DX) << 16) | reg_w(R_AX);
        break;
    case 4:
        src = (static_cast<uint64_t>(reg_w(R_DX)) << 32) | reg_w(R_AX);
    }
    uint32_t quotient = src / val;
    uint32_t remainder = src % val;
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
    print_instr(ctx, "div");
}

TEMPLATE_INSTRUCTION_HELPER(mul) {
    int len = decode_operands(ctx, eip);
    uint64_t result = ctx.operands[0].getUnsignedValue();
    uint32_t upper = 0;
    switch (ctx.operands[0].size) {
    case 1:
        result *= reg_b(R_AL);
        reg_w(R_AX) = result & 0xffff;
        upper = (result >> 8) & 0xff;
        break;
    case 2:
        result *= reg_w(R_AX);
        reg_w(R_AX) = result & 0xffff;
        upper = (result >> 16) & 0xffff;
        reg_w(R_DX) = upper;
        break;
    case 4:
        result *= reg_l(R_EAX);
        reg_l(R_EAX) = result & 0xffffffff;
        upper = (result >> 32) & 0xffffffff;
        reg_l(R_EDX) = upper;
    }
    if (upper) cpu.of = cpu.cf = 1;
    else cpu.of = cpu.cf = 0;
    return len;
}

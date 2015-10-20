TEMPLATE_INSTRUCTION_HELPER(imul) {
    if (ctx.operands[1].type == opt_undefined) {
        // 1 operand
        int32_t val = ctx.operands[0].getSignedValue();
        int64_t result = val;
        int32_t upper = 0;
        switch (ctx.operands[0].size) {
        case 1:
            result *= static_cast<int8_t>(reg_b(R_AL));
            reg_w(R_AX) = result & 0xffff;
            cpu.sf = (result >> 7) & 1;
            upper = (result >> 8) & 0xff;
            break;
        case 2:
            result *= static_cast<int16_t>(reg_w(R_AX));
            reg_w(R_AX) = result & 0xffff;
            cpu.sf = (result >> 15) & 1;
            upper = (result >> 16) & 0xffff;
            reg_w(R_DX) = upper;
            break;
        case 4:
            result *= static_cast<int32_t>(reg_l(R_EAX));
            reg_l(R_EAX) = result & 0xffffffff;
            cpu.sf = (result >> 31) & 1;
            upper = (result >> 32) & 0xffffffff;
            reg_l(R_EDX) = upper;
        }
        if (cpu.sf ? ~upper : upper) cpu.of = cpu.cf = 1;
        else cpu.of = cpu.cf = 0;
    } else if (ctx.operands[2].type == opt_undefined) {
        // 2 operands
        int32_t val1 = ctx.operands[0].getSignedValue();
        int32_t val2 = ctx.operands[1].getSignedValue();
        int64_t result = static_cast<int64_t>(val1) * val2;
        ctx.operands[0].setValue(result);
        cpu.sf = result >> (ctx.operands[0].size * 8 - 1);
        int32_t upper = result >> (ctx.operands[0].size * 8);
        if (cpu.sf ? ~upper : upper) cpu.of = cpu.cf = 1;
        else cpu.of = cpu.cf = 0;
    } else {
        // 3 operands
        int32_t val1 = ctx.operands[1].getSignedValue();
        int32_t val2 = ctx.operands[2].getSignedValue();
        int64_t result = static_cast<int64_t>(val1) * val2;
        ctx.operands[0].setValue(result);
        cpu.sf = result >> (ctx.operands[0].size * 8 - 1);
        int32_t upper = result >> (ctx.operands[0].size * 8);
        if (cpu.sf ? ~upper : upper) cpu.of = cpu.cf = 1;
        else cpu.of = cpu.cf = 0;
    }
}

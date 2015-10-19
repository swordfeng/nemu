TEMPLATE_INSTRUCTION_HELPER(ret_near) {
    decode_operands(ctx, eip);
    uint32_t temp_eip;
    if (ctx.prefix[prefix_operand]) {
        temp_eip = swaddr_read(reg_l(R_ESP), 2);
        reg_l(R_ESP) += 2;
    } else {
        temp_eip = swaddr_read(reg_l(R_ESP), 4);
        reg_l(R_ESP) += 4;
    }
    if (ctx.operands[0].type != opt_undefined) {
        // Stack address size is not detected
        reg_l(R_ESP) += ctx.operands[0].getUnsignedValue();
    }
    return temp_eip - eip;
}
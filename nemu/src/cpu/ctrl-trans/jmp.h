TEMPLATE_INSTRUCTION_HELPER(jmp_near) {
    int len = decode_operands(ctx, eip);
    uint32_t temp_eip = 0;
    if (op_name_is(op, imm)) {
        // rel8 / rel16 / rel32 relative address
        temp_eip = eip + len + ctx.operands[0].getSignedValue();
    } else {
        // r/m absolute address
        temp_eip = ctx.operands[0].getUnsignedValue();
    }
    if (ctx.operands[0].size == 16) temp_eip &= 0xffff;

    print_asm("jmp\t%x", temp_eip);

    // eip <- temp_eip;
    return temp_eip - eip;
}

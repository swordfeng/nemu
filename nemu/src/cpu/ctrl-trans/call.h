template <OperandName op>
HELPER(call_near) {
    int len = decode_operands<op>(ctx, eip);
    uint32_t temp_eip = 0;
    if (op_name_is(op, imm)) {
        // rel16 / rel32 relative address
        temp_eip = eip + len + ctx.operands[0].getUnsignedValue();
    } else {
        // r/m absolute address
        temp_eip = ctx.operands[0].getUnsignedValue();
    }
    temp_eip = int_trunc(temp_eip, ctx.operands[0].size);
    // push eip/ip
    reg_l(R_ESP) -= ctx.operands[0].size;
    swaddr_write(reg_l(R_ESP), ctx.operands[0].size, int_trunc(eip + len, ctx.operands[0].size));

    print_asm("call %x", temp_eip);

    // eip <- temp_eip;
    return temp_eip - eip;
}

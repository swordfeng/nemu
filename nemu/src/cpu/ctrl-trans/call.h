TEMPLATE_INSTRUCTION_HELPER(call_near) {
    uint32_t temp_eip;
    if (ctx.operands[0].type == opt_immediate) {
        // rel16 / rel32 relative address
        temp_eip = cpu.eip + ctx.operands[0].getUnsignedValue();
    } else {
        // r/m absolute address
        temp_eip = ctx.operands[0].getUnsignedValue();
    }
    temp_eip = int_trunc(temp_eip, ctx.operands[0].size);
    // push eip/ip
    reg_l(R_ESP) -= ctx.operands[0].size;
    swaddr_write(reg_l(R_ESP), ctx.operands[0].size, sreg_index(ss), cpu.eip);

    print_asm("call\t%x", temp_eip);

    // eip <- temp_eip;
    cpu.eip = temp_eip;
}

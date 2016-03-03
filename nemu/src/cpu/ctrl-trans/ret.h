TEMPLATE_INSTRUCTION_HELPER(ret_near) {
    uint32_t temp_eip;
    if (ctx.prefix[prefix_operand]) {
        temp_eip = swaddr_read(reg_l(R_ESP), 2, sreg_index(ss));
        reg_l(R_ESP) += 2;
    } else {
        temp_eip = swaddr_read(reg_l(R_ESP), 4, sreg_index(ss));
        reg_l(R_ESP) += 4;
    }
    if (ctx.operands[0].type != opt_undefined) {
        // Stack address size is not detected
        reg_l(R_ESP) += ctx.operands[0].getUnsignedValue();
    }
    print_instr(ctx, "ret");
    cpu.eip = temp_eip;
}

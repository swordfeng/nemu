TEMPLATE_INSTRUCTION_HELPER(jmp_near) {
    uint32_t temp_eip = 0;
    if (ctx.operands[0].type == opt_immediate) {
        // rel8 / rel16 / rel32 relative address
        temp_eip = cpu.eip + ctx.operands[0].getSignedValue();
    } else {
        // r/m absolute address
        temp_eip = ctx.operands[0].getUnsignedValue();
    }
    if (ctx.operands[0].size == 16) temp_eip &= 0xffff;

    print_asm("jmp\t%x", temp_eip);

    // eip <- temp_eip;
    cpu.eip = temp_eip;
}

TEMPLATE_INSTRUCTION_HELPER(jmp_long) {
    if (ctx.operands[0].type == opt_immediate) {
        Assert(ctx.operands[1].type == opt_immediate, "the second operand not present?!");
        swaddr_t temp_eip = ctx.operands[0].getUnsignedValue();
        cpu.cs.sel = ctx.operands[1].getUnsignedValue();
        sreg_load(sreg_index(cs));

        print_instr(ctx, "ljmp");

        cpu.eip = temp_eip;
    } else {
        panic("not implemented");
    }
}


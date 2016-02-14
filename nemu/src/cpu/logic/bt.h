TEMPLATE_INSTRUCTION_HELPER(bt) {
    if (ctx.operands[0].type == opt_register) {
        uint32_t offset = ctx.operands[1].getUnsignedValue() % (ctx.operands[0].size * 8);
        cpu.cf = ctx.operands[0].getUnsignedValue() >> offset;
    } else { // address
        uint32_t offset = ctx.operands[1].getUnsignedValue();
        uint32_t base = ctx.operands[0].getAddress() + offset / (ctx.operands[0].size * 8);
        offset %= ctx.operands[0].size * 8;
        cpu.cf = swaddr_read(base, ctx.operands[0].size, ctx.operands[0].sreg) >> offset;
    }
    print_instr(ctx, "bt");
}


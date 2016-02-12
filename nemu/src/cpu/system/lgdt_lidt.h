TEMPLATE_INSTRUCTION_HELPER(lgdt) {
    uint32_t addr = ctx.operands[0].getAddress();
    uint8_t sreg = ctx.operands[0].getSreg();
    cpu.gdtr.limit = swaddr_read(addr, 2, sreg);
    addr += 2;
    if (ctx.prefix[2] == prefix_2_operand) { // operand size = 16
        cpu.gdtr.base = swaddr_read(addr, 4, sreg) & 0xFFFFFF;
    } else {
        cpu.gdtr.base = swaddr_read(addr, 4, sreg);
    }
    print_instr(ctx, "lgdt");
}


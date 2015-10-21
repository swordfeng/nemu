TEMPLATE_INSTRUCTION_HELPER(shld) {
    uint32_t bits = ctx.operands[2].getUnsignedValue() & 0x1f;
    uint64_t value = ctx.operands[0].getUnsignedValue();
    size_t size = ctx.operands[0].size;
    value = (value << size * 8) | ctx.operands[1].getUnsignedValue();
    uint32_t result = value >> (size * 8 - bits);
    ctx.operands[0].setValue(result);
    if (bits) {
        cpu.sf = result >> (size * 8 - 1);
        cpu.zf = result == 0;
        cpu.pf = calc_pf(result);
        cpu.cf = value >> (size * 16 - bits);
    }
    if (bits == 1) cpu.of = cpu.cf != cpu.sf;
    print_instr(ctx, "shld");
}

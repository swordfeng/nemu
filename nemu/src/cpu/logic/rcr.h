TEMPLATE_INSTRUCTION_HELPER(rcr) {
    uint32_t origbits = ctx.operands[1].getUnsignedValue();
    uint32_t value = ctx.operands[0].getUnsignedValue();
    size_t size = ctx.operands[0].size;
    uint32_t bits = (origbits & 0x1f) % (size * 8 + 1);
    if (origbits == 1) cpu.of = cpu.cf ^ (value >> (size * 8 - 1));
    uint32_t result = 0;
    if (bits < 32) result |= (value >> bits);
    if (bits > 1) result |= (value << (size * 8 + 1 - bits));
    if (bits != 0) {
        result |= cpu.cf << (size * 8 - bits);
        cpu.cf = value >> (bits - 1);
    }
    ctx.operands[0].setValue(result);
    print_instr(ctx, "rcr");
}

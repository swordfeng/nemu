TEMPLATE_INSTRUCTION_HELPER(rcl) {
    uint32_t origbits = ctx.operands[1].getUnsignedValue();
    uint32_t value = ctx.operands[0].getSignedValue();
    size_t size = ctx.operands[0].size;
    uint32_t bits = (origbits & 0x1f) % (size * 8 + 1);
    uint32_t result = 0;
    if (bits < 32) result |= value << bits;
    if (bits > 1) result |= value >> (size * 8 + 1 - bits);
    if (bits != 0) {
        result |= cpu.cf << (bits - 1);
        cpu.cf = value >> (size * 8 - bits);
    }
    ctx.operands[0].setValue(result);
    if (origbits == 1) cpu.of = cpu.cf != result >> (size * 8 - 1);
    print_instr(ctx, "rcl");
}

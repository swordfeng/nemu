TEMPLATE_INSTRUCTION_HELPER(ror) {
    uint32_t origbits = ctx.operands[1].getUnsignedValue() & 0x1f;
    uint32_t value = ctx.operands[0].getUnsignedValue();
    size_t size = ctx.operands[0].size;
    uint32_t bits = origbits % (size * 8);
    uint32_t result = (value >> bits) | (value << (size * 8 - bits));
    ctx.operands[0].setValue(result);
    cpu.cf = value >> (size * 8 - 1);
    if (origbits == 1) cpu.of = cpu.cf ^ (value >> (size * 8 - 2));
    print_instr(ctx, "ror");
}

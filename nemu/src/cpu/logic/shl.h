TEMPLATE_INSTRUCTION_HELPER(shl) {
    uint32_t bits = ctx.operands[1].getUnsignedValue() & 0x1f;
    uint32_t value = ctx.operands[0].getSignedValue();
    size_t size = ctx.operands[0].size;
    uint32_t result = value << bits;
    ctx.operands[0].setValue(result);
    cpu.sf = result >> (size * 8 - 1);
    cpu.zf = result == 0;
    cpu.pf = calc_pf(result);
    if (bits) cpu.cf = value >> (size * 8 - bits);
    if (bits == 1) cpu.of = cpu.cf != cpu.sf;
    print_instr(ctx, "shl");
}

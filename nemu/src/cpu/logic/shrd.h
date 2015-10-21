TEMPLATE_INSTRUCTION_HELPER(shrd) {
    uint32_t bits = ctx.operands[2].getUnsignedValue() & 0x1f;
    uint64_t value = ctx.operands[0].getUnsignedValue();
    size_t size = ctx.operands[0].size;
    value |= static_cast<uint64_t>(ctx.operands[1].getUnsignedValue()) << (size * 8);
    uint32_t result = value >> bits;
    ctx.operands[0].setValue(result);
    if (bits) {
        cpu.sf = result >> (size * 8 - 1);
        cpu.zf = result == 0;
        cpu.pf = calc_pf(result);
        cpu.cf = value >> (bits - 1);
    }
    if (bits == 1) cpu.of = value >> (size * 8 - 1);
    print_instr(ctx, "shrd");
}

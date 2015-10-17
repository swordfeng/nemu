TEMPLATE_INSTRUCTION_HELPER(nop) {
    int len = decode_operands(ctx, eip);
    print_instr(ctx, "nop");
    return len;
}

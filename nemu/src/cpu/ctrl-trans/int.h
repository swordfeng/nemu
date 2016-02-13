extern "C" void do_int3();

INSTRUCTION_HELPER(int3) {
	print_asm("int3");
	do_int3();
}

TEMPLATE_INSTRUCTION_HELPER(intr) {
    print_instr(ctx, "int");
    raise_intr(ctx.operands[0].getUnsignedValue());
}

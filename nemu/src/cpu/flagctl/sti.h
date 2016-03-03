INSTRUCTION_HELPER(sti) {
    cpu.ief = 1;
    print_instr(ctx, "sti");
}

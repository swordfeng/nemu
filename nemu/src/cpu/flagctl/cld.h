INSTRUCTION_HELPER(cld) {
    // DF <- 0
    cpu.df = 0;
    print_instr(ctx, "cld");
}

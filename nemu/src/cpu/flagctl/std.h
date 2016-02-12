INSTRUCTION_HELPER(stdf) {
    // DF <- 1
    cpu.df = 1;
    print_instr(ctx, "std");
}

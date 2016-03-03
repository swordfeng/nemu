INSTRUCTION_HELPER(cli) {
    cpu.ief = 0;
    print_instr(ctx, "cli");
}

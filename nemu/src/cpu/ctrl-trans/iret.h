INSTRUCTION_HELPER(iret) {
    Assert(ctx.prefix[prefix_operand] == 0, "16-bit not supported");
    print_instr(ctx, "iretd");
    Assert(cpu.cr0.pe == 1, "pe = 0 not handled");
    cpu.eip = swaddr_read(cpu.esp, 4, sreg_index(ss));
    cpu.esp += 4;
    cpu.cs.sel = (uint16_t) swaddr_read(cpu.esp, 4, sreg_index(ss));
    cpu.esp += 4;
    cpu.eflags = swaddr_read(cpu.esp, 4, sreg_index(ss));
    cpu.esp += 4;
    sreg_load_name(cs);
    Assert(cpu.eip < cpu.cs.limit, "eip out of cs limit");
}

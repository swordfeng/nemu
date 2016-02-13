INSTRUCTION_HELPER(popa) {
    size_t sz = ctx.prefix[prefix_operand] ? 2 : 4;
    uint32_t temp = cpu.esp;
    if (sz == 2) {
        for (size_t index = 8; index + 1 != 0; index--) {
            cpu.gpr[index]._16 = swaddr_read(temp, sz, sreg_index(ss));
            temp += sz;
        }
    } else {
        for (size_t index = 8; index + 1 != 0; index--) {
            cpu.gpr[index]._32 = swaddr_read(temp, sz, sreg_index(ss));
            temp += sz;
        }
    }
    cpu.esp = temp;
    print_asm(sz == 2 ? "popa" : "popad");
}

INSTRUCTION_HELPER(pusha) {
    size_t sz = ctx.prefix[prefix_operand] ? 2 : 4;
    uint32_t temp = cpu.esp;
    for (size_t index = 0; index < 8; index++) {
        temp -= sz;
        swaddr_write(temp, sz, sreg_index(ss), cpu.gpr[index]._32);
    }
    cpu.esp = temp;
    print_asm(sz == 2 ? "pusha" : "pushad");
}

INSTRUCTION_HELPER(hlt) {
    if (!cpu.INTR) cpu.eip--;
    print_asm("hlt");
}


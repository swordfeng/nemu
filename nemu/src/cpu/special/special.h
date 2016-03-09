extern "C" {
#include "monitor/monitor.h"
}

INSTRUCTION_HELPER(inv) {
    /* invalid opcode */

    eip = ctx.decoded_eip;
    uint32_t temp[8];
    temp[0] = instr_fetch(eip, 4);
    temp[1] = instr_fetch(eip + 4, 4);

    uint8_t *p = (uint8_t *)temp;
    printf("invalid opcode(eip = 0x%08x): %02x %02x %02x %02x %02x %02x %02x %02x ...\n\n",
            eip, p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);

    extern unsigned char logo [];
    printf("There are two cases which will trigger this unexpected exception:\n"
           "1. The instruction at eip = 0x%08x is not implemented.\n"
           "2. Something is implemented incorrectly.\n", eip);
    printf("Find this eip value(0x%08x) in the disassembling result to distinguish which case it is.\n\n", eip);
    printf("\33[1;31mIf it is the first case, see\n%s\nfor more details.\n\nIf it is the second case, remember:\n"
           "* The machine is always right!\n"
           "* Every line of untested code is always wrong!\33[0m\n\n", logo);
    nemu_state = END;
}

INSTRUCTION_HELPER(nemu_trap) {
    print_asm("nemu_trap (eax = %d)", cpu.eax);

    uint32_t buf = cpu.ecx, len = cpu.edx;
    switch(cpu.eax) {
        case 2:
            for (uint32_t i = 0; i < len; i++) putchar(swaddr_read(buf + i, 1, sreg_index(ds)));
               break;

        default:
            printf("\33[1;31mnemu: HIT %s TRAP\33[0m at eip = 0x%08x\n\n",
                    (cpu.eax == 0 ? "GOOD" : "BAD"), eip);
            nemu_state = END;
    }
}


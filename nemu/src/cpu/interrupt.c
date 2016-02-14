#include "common.h"
#include "cpu/reg.h"
#include "memory/memory.h"
#include <setjmp.h>

extern jmp_buf jbuf;

// 0 <- others, 1 <- fault(eip of current instruction), 2 <- trap(next instruction eip), 3 <- abort
//static int8_t interrupt_types[] = {1, 0, 2, 2, 2, 1, 1, 1, 3, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1}; // up to 20

void raise_intr(uint8_t NO) {
    Assert(NO <= cpu.idtr.limit, "Interrupt number exceeded");
    lnaddr_t pidt = cpu.idtr.base + NO * 8;
    uint64_t idt_des = ((uint64_t) lnaddr_read(pidt + 4, 4) << 32) | lnaddr_read(pidt, 4); 
    Assert((idt_des >> 47) & 1, "IDT descripter does not present, Interrupt # = %#x", NO);
    uint8_t gate_type = (idt_des >> 40) & 0x7;
    //uint8_t gate_size = (idt_des >> 43) & 1;
    Assert(gate_type != 5, "task gate not implemented");
    if (NO != 2 && !cpu.ief) return; // not NMI, IF is not present
    // push registers
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, sreg_index(ss), cpu.eflags);
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, sreg_index(ss), cpu.cs.sel);
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, sreg_index(ss), cpu.eip); // next instruction now
    // no error code currently
    if (gate_type == 6) { // interrupt gate
        cpu.ief = 0;
    }
    // long jump
    cpu.cs.sel = (idt_des >> 16) & 0xFFFF;
    sreg_load_name(cs);
    cpu.eip = (idt_des & 0xFFFF) | ((idt_des >> 32LL) & 0xFFFF0000);
    /* Jump back to cpu_exec() */
    longjmp(jbuf, 1);
}

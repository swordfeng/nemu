#include "irq.h"

#include <sys/syscall.h>

void add_irq_handle(int, void (*)(void));
void mm_brk(uint32_t);

static void sys_brk(TrapFrame *tf) {
#ifdef IA32_PAGE
	mm_brk(tf->ebx);
#endif
	tf->eax = 0;
}

static inline void sys_write(TrapFrame *tf) {
    uint32_t fd = tf->ebx;
    uint32_t buf = tf->ecx;
    uint32_t len = tf->edx;
    if (fd != 1 && fd != 2) panic("unsupported file descripter");
    asm volatile(".byte 0xd6" : : "a"(2), "c"(buf), "d"(len));
    tf->eax = len;
}

void do_syscall(TrapFrame *tf) {
	switch(tf->eax) {
		/* The ``add_irq_handle'' system call is artificial. We use it to 
		 * let user program register its interrupt handlers. But this is 
		 * very dangerous in a real operating system. Therefore such a 
		 * system call never exists in GNU/Linux.
		 */
		case 0: 
			cli();
			add_irq_handle(tf->ebx, (void*)tf->ecx);
			sti();
			break;

		case SYS_brk: sys_brk(tf); break;

		/* TODO: Add more system calls. */
        case SYS_write: sys_write(tf); break;

		default: panic("Unhandled system call: id = %d", tf->eax);
	}
}


#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "memory/memory.h"
#include "nemu.h"

#include <stdlib.h>
#include <ctype.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);
const char *elf_find_func(swaddr_t addr);

typedef struct {
    swaddr_t prev_ebp;
	swaddr_t ret_addr;
	uint32_t args[4];
} StackFrameBottom;

/* We use the ``readline'' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args) {
	uint32_t ins = 1;
	if (args) {
		if (0 == sscanf(args, "%u", &ins)) {
			printf("Invalid argument: %s\n", args);
			return 0;
		}
	}
	cpu_exec(ins);
	return 0;
}

static int cmd_info_r();
static int cmd_info_w();
static int cmd_info(char *args) {
	if (!args) {
		printf("info r\t\tPrint registers\ninfo w\t\tPrint watchpoints\n");
	} else {
		switch (args[0]) {
			case 'r': return cmd_info_r();
			case 'w': return cmd_info_w();
		}
		printf("Invalid argmuent: %s\n", args);
	}
	return 0;
}

static int cmd_info_r() {
	int i;
	for (i = 0; i < 8; i++) {
		printf("%s\t%#10x\t%10d\t", regsl[i], reg_l(i), reg_l(i));
		printf("%s\t%#6x\t%5d", regsw[i], reg_w(i), reg_w(i));
		if (i < 4) {
			printf("\t%s\t%#4x\t%3d\t", regsb[i|4], reg_b(i|4), reg_b(i|4));
			printf("%s\t%#4x\t%3d\n", regsb[i], reg_b(i), reg_b(i));
		} else printf("\n");
	}
    for (i = 0; i < 6; i++) {
        printf("%s\t%#6x: base = %#10x, limit = %#10x\n", regss[i], cpu.sr[i].sel, cpu.sr[i].base, cpu.sr[i].limit);
    }
	printf("eflags\t%#10x [%s%s%s%s%s%s%s ]\n", cpu.eflags,
			cpu.cf ? "CF" : "",
			cpu.pf ? "PF" : "",
			cpu.zf ? "ZF" : "",
			cpu.sf ? "SF" : "",
			cpu.ief? "IF" : "",
			cpu.df ? "DF" : "",
			cpu.of ? "OF" : ""
	);
	printf("eip\t%#10x\n", cpu.eip);
    printf("cr0\t%#10x\tcr3\t%#10x\n", cpu.cr0.value, cpu.cr3);
    printf("gdtr:\tbase = %#10x, limit = %#10x\nidtr:\tbase = %#10x, limit = %#10x\n", cpu.gdtr.base, cpu.gdtr.limit, cpu.idtr.base, cpu.idtr.limit);
	return 0;
}

static int cmd_info_w() {
	wp_print_info();
	return 0;
}

static int cmd_x(char *args) {
	size_t ins = 0;
	swaddr_t addr;
	while (args && *args && *args == ' ') args++;
	for (; args && *args && isdigit(*args); args++) {
		ins = ins * 10 + *args - '0';
	}
	while (args && *args && *args == ' ') args++;
	if (!args || !*args) {
		printf("invalid arguments\n");
		return 0;
	}
	bool success = false;
	addr = expr(args, &success);
	if (!success) return 0;
	while (ins--) {
		printf("%#x:\t0x%08x\n", addr, swaddr_read(addr, 4, sreg_index(ds)));
		addr += 4;
	}
	return 0;
}

static int cmd_p(char *args) {
	while (*args == ' ') args++;
	char *args_end = args + strlen(args) - 1;
	while (args_end > args && *args_end == ' ') args_end--;
	int len = args_end - args + 1;
	if (len <= 0) {
		printf("Need expression!\n");
		return 0;
	}
	char *expstr = malloc(len + 1);
	strncpy(expstr, args, len + 1);
	bool success = false;
	uint32_t result = expr(expstr, &success);
	if (success) printf("%#x\t(%u)\n", result, result);
	return 0;
}

static int cmd_w(char *args) {
    if (!args) {
        printf("invalid argument\n");
        return 0;
    }
	while (*args == ' ') args++;
	char *newexp;
	bool valid = expr_prettify(args, &newexp);
	if (!valid) {
		printf("failed to set watchpoint\n");
		return 0;
	}
	WP *wp = wp_new();
	wp_set_expr(wp, newexp);
	printf("New watchpoint %d: %s\n", wp_get_no(wp), newexp);
	return 0;
}

static int cmd_d(char *args) {
	int wp_no;
	if (1 != sscanf(args, "%d", &wp_no)) {
		printf("Invalid argument\n");
		return 0;
	}
	WP *wp;
	if (!(wp = wp_find(wp_no))) {
		printf("Invalid watchpoint number\n");
		return 0;
	}
	wp_free(wp);
	return 0;
}

static int cmd_bt(char *args) {
	StackFrameBottom st;
	st.prev_ebp = cpu.ebp;
	st.ret_addr = cpu.eip;
	int count = 0;
	do {
		swaddr_t eip_temp = st.ret_addr;
		swaddr_t ebp_temp = st.prev_ebp;
		swaddr_read_bytes(&st, ebp_temp, sizeof(StackFrameBottom), sreg_index(ss));
		const char *func_name = elf_find_func(eip_temp);
		if (func_name == NULL) func_name = "??";
		printf("#%d\t0x%08x in %s (%#x, %#x, %#x, %#x)\n", count, eip_temp, func_name, st.args[0]
			, st.args[1], st.args[2], st.args[3]);
		count++;
	} while (st.prev_ebp);
	return 0;
}

#ifdef USE_CACHE
static int cmd_cache(char *args) {
    bool succ = false;
    hwaddr_t addr = expr(args, &succ);
    if (!succ) {
        printf("invalid argument\n");
        return 1;
    }
    cache_show(addr);
    return 0;
}
#endif

static int cmd_page(char *args) {
    bool succ = false;
    lnaddr_t lnaddr = expr(args, &succ);
    if (!succ) {
        printf("invalid address\n");
        return 1;
    }
    page_show(lnaddr);
    return 0;
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{ "si", "Step assembly", cmd_si },
	{ "info", "Print program info", cmd_info },
	{ "x", "Examine memory", cmd_x },
	{ "p", "Calculate and print expression", cmd_p },
	{ "w", "Set new watchpoint", cmd_w },
	{ "d", "Delete new watchpoint", cmd_d },
	{ "bt", "Display Back Trace", cmd_bt},
#ifdef USE_CACHE
	{ "cache", "Display Back Trace", cmd_cache},
#endif
	{ "page", "Display Page Translation", cmd_page},
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		if (!str) return;
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}

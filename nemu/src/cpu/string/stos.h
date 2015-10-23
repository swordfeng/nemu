INSTRUCTION_HELPER(stos) {
    size_t size;
    if (ctx.opcode & 1) { /* AB */
        if (ctx.prefix[2]) {
            size = 2;
        } else {
            size = 4;
        }
    } else {
        size = 1;
    }
    size_t len = 1;
    if (ctx.prefix[prefix_address]) {
        if (ctx.prefix[0] == prefix_0_rep) {
            len = reg_w(R_CX);
        }
        while (len--) {
            uint32_t val = reg_read_index(R_EAX, size);
            swaddr_write(reg_w(R_DI), size, val);
            if (cpu.df) {
                reg_w(R_DI) -= size;
            } else {
                reg_w(R_DI) += size;
            }
        }
    } else { /* AA */
        if (ctx.prefix[0] == prefix_0_rep) {
            len = reg_l(R_ECX);
        }
        while (len--) {
            uint32_t val = reg_read_index(R_EAX, size);
            swaddr_write(reg_l(R_EDI), size, val);
            if (cpu.df) {
                reg_l(R_EDI) -= size;
            } else {
                reg_l(R_EDI) += size;
            }
        }
    }
    switch (size) {
    case 1:
        print_instr(ctx, "stosb");
        break;
    case 2:
        print_instr(ctx, "stosw");
        break;
    case 4:
        print_instr(ctx, "stosl");
        break;
    }
}

INSTRUCTION_HELPER(movs) {
    size_t size;
    if (ctx.opcode & 1) { /* A5 */
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
            uint32_t val = swaddr_read(reg_w(R_SI), size);
            swaddr_write(reg_w(R_DI), size, val);
            if (cpu.df) {
                reg_w(R_SI) -= size;
                reg_w(R_DI) -= size;
            } else {
                reg_w(R_SI) += size;
                reg_w(R_DI) += size;
            }
        }
    } else {
        if (ctx.prefix[0] == prefix_0_rep) {
            len = reg_l(R_ECX);
        }
        while (len--) {
            uint32_t val = swaddr_read(reg_l(R_ESI), size);
            swaddr_write(reg_l(R_EDI), size, val);
            if (cpu.df) {
                reg_l(R_ESI) -= size;
                reg_l(R_EDI) -= size;
            } else {
                reg_l(R_ESI) += size;
                reg_l(R_EDI) += size;
            }
        }
    }
    switch (size) {
    case 1:
        print_instr(ctx, "movsb");
        break;
    case 2:
        print_instr(ctx, "movsw");
        break;
    case 4:
        print_instr(ctx, "movsl");
        break;
    }
}

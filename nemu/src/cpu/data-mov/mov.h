#pragma once

#include "cpu/exec.hh"
#include "cpu/decode.hh"

TEMPLATE_HELPER(mov) {
    int len = decode_operands<operand_names...>(ctx, eip);
    ctx.operands[0].setValue(ctx.operands[1].getValue());
    string instr_name = "mov";
    if (ctx.operands[0].type == opt_address &&
        (ctx.operands[1].type == opt_address || ctx.operands[1].type == opt_immediate)) {
        instr_name += ctx.operands[0].suffix();
    }
    print_instr(ctx, instr_name);
    return len;
}

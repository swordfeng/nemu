#pragma once

#include "cpu/exec.hh"
#include "cpu/decode.hh"

template <OperandName op1, OperandName op2>
HELPER(sub) {
    int len = decode_operands<op1, op2>(ctx, eip);
    uint32_t val1 = ctx.operands[0].getValue();
    uint32_t val2 = ctx.operands[1].getValue();
    if (ctx.operands[1].size < ctx.operands[0].size) val2 = signed_extend(val2);
    ctx.operands[0].setValue(val1 - val2);
    // TODO: set EFLAGS
    print_instr(ctx, "sub");
    return len;
}

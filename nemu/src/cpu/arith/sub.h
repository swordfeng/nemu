#pragma once

#include "cpu/decode.hh"

template <OperandName op1, OperandName op2>
HELPER(sub) {
    int len = decode_operands<op1, op2>(ctx, eip);
    uint32_t val1 = ctx.operands[0].getSignedValue();
    uint32_t val2 = ctx.operands[1].getSignedValue();
    uint32_t result = int_trunc(val1 - val2, ctx.operands[0].size);
    ctx.operands[0].setValue(result);
    // TODO: set EFLAGS
    cpu.cf = result > val2;
    cpu.zf = result == 0;
    print_instr(ctx, "sub");
    return len;
}

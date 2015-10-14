#pragma once

#include "cpu/exec.hh"
#include "cpu/decode.hh"

template <OperandName op1, OperandName op2>
HELPER(mov) {
    int len = decode_operands<op1, op2>(ctx, eip);
    ctx.operands[0].setValue(ctx.operands[1].getValue());
    print_instr(ctx, "mov");
    return len;
}

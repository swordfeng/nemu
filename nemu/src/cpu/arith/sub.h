#pragma once

#include "cpu/decode.hh"

TEMPLATE_INSTRUCTION_HELPER(sub) {
    int len = decode_operands(ctx, eip);
    uint32_t val1 = ctx.operands[0].getSignedValue();
    uint32_t val2 = ctx.operands[1].getSignedValue();
    uint32_t result = val1 - val2;
    size_t result_size = ctx.operands[0].size;
    ctx.operands[0].setValue(result);
    // TODO: set EFLAGS
    cpu.cf = int_trunc(result, result_size) > int_trunc(val1, result_size);
    cpu.zf = result == 0;
    cpu.sf = result >> 31;
    cpu.of = (val1 >> 31) != (val2 >> 31) && (val1 >> 31) != (result >> 31);
    cpu.pf = calc_pf(result);
    // AF ignored
    print_instr(ctx, "sub");
    return len;
}

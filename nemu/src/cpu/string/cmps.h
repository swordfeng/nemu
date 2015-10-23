#include "rep.h"

REP_INSTRUCTION_HELPER(cmps) {
    // copied from cmp.h
    uint32_t val1 = swaddr_read(ctx.operands[0].getUnsignedValue(), ctx.operands[2].size);
    uint32_t val2 = swaddr_read(ctx.operands[1].getUnsignedValue(), ctx.operands[2].size);
    size_t result_size = ctx.operands[0].size;
    uint32_t result = signed_extend(int_trunc(val1 - val2, result_size), result_size);

    cpu.cf = int_trunc(result, result_size) > int_trunc(val1, result_size);
    cpu.zf = result == 0;
    cpu.sf = result >> 31;
    cpu.of = (val1 >> 31) != (val2 >> 31) && (val1 >> 31) != (result >> 31);
    cpu.pf = calc_pf(result);
    if (cpu.df) {
        ctx.operands[1].setValue(ctx.operands[1].getUnsignedValue() - ctx.operands[2].size);
        ctx.operands[0].setValue(ctx.operands[0].getUnsignedValue() - ctx.operands[2].size);
    } else {
        ctx.operands[1].setValue(ctx.operands[1].getUnsignedValue() + ctx.operands[2].size);
        ctx.operands[0].setValue(ctx.operands[0].getUnsignedValue() + ctx.operands[2].size);
    }
    return ctx.prefix[0] == prefix_0_repe ? !cpu.zf : cpu.zf;
}

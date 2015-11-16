#include "rep.h"
#include "../general-add.h"

REP_INSTRUCTION_HELPER(cmps) {
    // copied from cmp.h
    uint32_t val1 = swaddr_read(ctx.operands[0].getUnsignedValue(), ctx.operands[2].size);
    uint32_t val2 = swaddr_read(ctx.operands[1].getUnsignedValue(), ctx.operands[2].size);
    size_t result_size = ctx.operands[0].size;
    cpu.cf = 1;
    general_add(val1, ~val2, result_size);
    if (cpu.df) {
        ctx.operands[1].setValue(ctx.operands[1].getUnsignedValue() - ctx.operands[2].size);
        ctx.operands[0].setValue(ctx.operands[0].getUnsignedValue() - ctx.operands[2].size);
    } else {
        ctx.operands[1].setValue(ctx.operands[1].getUnsignedValue() + ctx.operands[2].size);
        ctx.operands[0].setValue(ctx.operands[0].getUnsignedValue() + ctx.operands[2].size);
    }
    return ctx.prefix[0] == prefix_0_repne ? !cpu.zf : cpu.zf;
}

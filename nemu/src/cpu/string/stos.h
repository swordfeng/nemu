#include "rep.h"

REP_INSTRUCTION_HELPER(stos) {
    uint32_t val = reg_read_index(R_EAX, ctx.operands[2].size);
    swaddr_write(ctx.operands[0].getUnsignedValue(), ctx.operands[2].size, val, sreg_index(es));
    if (cpu.df) {
        ctx.operands[0].setValue(ctx.operands[0].getUnsignedValue() - ctx.operands[2].size);
    } else {
        ctx.operands[0].setValue(ctx.operands[0].getUnsignedValue() + ctx.operands[2].size);
    }
    return true;
}

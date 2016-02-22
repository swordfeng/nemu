#include "rep.h"

REP_INSTRUCTION_HELPER(movs) {
    uint32_t val = swaddr_read(ctx.operands[1].getUnsignedValue(), ctx.operands[2].size, sreg_index(ds));
    swaddr_write(ctx.operands[0].getUnsignedValue(), ctx.operands[2].size, sreg_index(es), val);
    if (cpu.df) {
        ctx.operands[1].setValue(ctx.operands[1].getUnsignedValue() - ctx.operands[2].size);
        ctx.operands[0].setValue(ctx.operands[0].getUnsignedValue() - ctx.operands[2].size);
    } else {
        ctx.operands[1].setValue(ctx.operands[1].getUnsignedValue() + ctx.operands[2].size);
        ctx.operands[0].setValue(ctx.operands[0].getUnsignedValue() + ctx.operands[2].size);
    }
//    Log("movs %x %x", ctx.operands[0].getUnsignedValue(), ctx.operands[1].getUnsignedValue());
    return true;
}

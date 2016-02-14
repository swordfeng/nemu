#include "device/port-io.h"
TEMPLATE_INSTRUCTION_HELPER(out) {
    print_instr(ctx, "out");
    pio_write(ctx.operands[0].getUnsignedValue(), ctx.operands[1].size, ctx.operands[1].getUnsignedValue());
}


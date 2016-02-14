#include "device/port-io.h"
TEMPLATE_INSTRUCTION_HELPER(in) {
    print_instr(ctx, "in");
    ctx.operands[0].setValue(pio_read(ctx.operands[1].getUnsignedValue(), ctx.operands[0].size));
}

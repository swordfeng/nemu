#include "../general-add.h"

TEMPLATE_INSTRUCTION_HELPER(cmp) {
    uint32_t val1 = ctx.operands[0].getSignedValue();
    uint32_t val2 = ctx.operands[1].getSignedValue();
    size_t result_size = ctx.operands[0].size;
    cpu.cf = 1;
    general_add(val1, ~val2, result_size);
    // AF ignored
    print_instr(ctx, "cmp");
}

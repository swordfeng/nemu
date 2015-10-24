#include "../general-add.h"

TEMPLATE_INSTRUCTION_HELPER(sbb) {
    uint32_t val1 = ctx.operands[0].getSignedValue();
    uint32_t val2 = ctx.operands[1].getSignedValue();
    size_t result_size = ctx.operands[0].size;
    cpu.cf ^= 1;
    uint32_t result = general_add(val1, ~val2, result_size);
    ctx.operands[0].setValue(result);
    cpu.cf ^= 1;
    // AF ignored
    print_instr(ctx, "sbb");
}

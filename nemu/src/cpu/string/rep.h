#pragma once

#define REP_INSTRUCTION_HELPER(name) \
bool name##_rep_internal HELPER_PARAM_LIST; \
INSTRUCTION_HELPER(name) { \
    size_t size; \
    if (ctx.opcode & 1) { \
        if (ctx.prefix[2]) { \
            size = 2; \
            print_instr(ctx, #name "w"); \
        } else { \
            size = 4; \
            print_instr(ctx, #name "l"); \
        } \
    } else { \
        size = 1; \
        print_instr(ctx, #name "b"); \
    } \
    ctx.operands[0].type = opt_register; \
    ctx.operands[0].reg_index = R_EDI; \
    ctx.operands[1].type = opt_register; \
    ctx.operands[1].reg_index = R_ESI; \
    ctx.operands[2].type = opt_immediate; \
    ctx.operands[2].size = size; \
    ctx.operands[3].type = opt_register; \
    ctx.operands[3].reg_index = R_ECX; \
    if (ctx.prefix[prefix_address]) { \
        ctx.operands[0].size = 2; \
        ctx.operands[1].size = 2; \
        ctx.operands[3].size = 2; \
    } else { \
        ctx.operands[0].size = 4; \
        ctx.operands[1].size = 4; \
        ctx.operands[3].size = 4; \
    } \
    uint32_t countReg; \
    if (ctx.prefix[prefix_lock_rep]) countReg = ctx.operands[3].getUnsignedValue(); \
    else countReg = 1; \
    bool cont = true; \
    while (countReg && cont) { \
        cont = name##_rep_internal(ctx, eip); \
        --countReg; \
    } \
    if (ctx.prefix[prefix_lock_rep]) ctx.operands[3].setValue(countReg); \
} \
bool name##_rep_internal HELPER_PARAM_LIST

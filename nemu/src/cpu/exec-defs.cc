#include "exec.hh"


Operand::Operand(): type(opt_undefined) {}

InstructionContext::InstructionContext():
    prefix({0, 0, 0, 0}), opcode(0), require_mod_rm(false),
    require_sib(false), disp_len(0), immediate_len(0) {}

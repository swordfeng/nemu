TEMPLATE_INSTRUCTION_HELPER(push) {
  // NOT IMPLEMENTED: Detection of Address size, Operand size, Stack-address Size.
  // See manual.
  Assert(ctx.operands[0].size == 2 || ctx.operands[0].size == 4, "unsupported data size");
  uint32_t val = ctx.operands[0].getUnsignedValue();
  reg_l(R_ESP) -= ctx.operands[0].size;
  swaddr_write(reg_l(R_ESP), ctx.operands[0].size, val);
  print_instr(ctx, "push");
}

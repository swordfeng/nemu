TEMPLATE_INSTRUCTION_HELPER(push) {
  int len = decode_operands(ctx, eip);
  // NOT IMPLEMENTED: Detection of Address size, Operand size, Stack-address Size.
  // See manual.
  Assert(ctx.operands[0].size == 2 || ctx.operands[0].size == 4, "unsupported data size");
  uint32_t val = ctx.operands[0].getUnsignedValue();
  reg_l(R_ESP) -= ctx.operands[0].size;
  swaddr_write(reg_l(R_ESP), ctx.operands[0].size, val);
  string instr_name = "push";
  if (ctx.operands[0].type != opt_register) instr_name += ctx.operands[0].suffix();
  print_instr(ctx, instr_name);
  return len;
}

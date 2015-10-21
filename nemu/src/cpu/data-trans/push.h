TEMPLATE_INSTRUCTION_HELPER(push) {
  // NOT IMPLEMENTED: Detection of Address size, Stack-address Size.
  // See manual.
  size_t data_size = ctx.prefix[PREFIX_DATA] ? 2 : 4;
  uint32_t val = ctx.operands[0].getSignedValue();
  reg_l(R_ESP) -= data_size;
  swaddr_write(reg_l(R_ESP), data_size, val);
  print_instr(ctx, "push");
}

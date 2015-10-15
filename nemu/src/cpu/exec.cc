#include "cpu/exec.hh"
#include "all-instr.h"

#include <vector>

/* opcode with reg/op */
helper_fun op_group(std::vector<helper_fun> fun_list);
/* 2-byte opcode */
HELPER(op_escape);
/* instruction prefix */
template <size_t prefix_group> HELPER(op_prefix);

/* TODO: Add more instructions!!! */

helper_fun opcode_table[256] = {
/* 0x00 */	inv, inv, inv, inv,
/* 0x04 */	inv, inv, inv, inv,
/* 0x08 */	inv, inv, inv, inv,
/* 0x0c */	inv, inv, inv, op_escape,
/* 0x10 */	inv, inv, inv, inv,
/* 0x14 */	inv, inv, inv, inv,
/* 0x18 */	inv, inv, inv, inv,
/* 0x1c */	inv, inv, inv, inv,
/* 0x20 */	inv, inv, inv, inv,
/* 0x24 */	inv, inv, inv, inv,
/* 0x28 */	&sub<op_rm_b, op_reg_b>, &sub<op_rm_v, op_reg_v>, &sub<op_reg_b, op_rm_b>, &sub<op_rm_v, op_reg_v>,
/* 0x2c */	&sub<op_a_b, op_imm_b>, &sub<op_a_v, op_imm_v>, inv, inv,
/* 0x30 */	inv, inv, inv, inv,
/* 0x34 */	inv, inv, inv, inv,
/* 0x38 */	inv, inv, inv, inv,
/* 0x3c */	inv, inv, inv, inv,
/* 0x40 */	inv, inv, inv, inv,
/* 0x44 */	inv, inv, inv, inv,
/* 0x48 */	inv, inv, inv, inv,
/* 0x4c */	inv, inv, inv, inv,
/* 0x50 */	&push<op_r_v>, &push<op_r_v>, &push<op_r_v>, &push<op_r_v>,
/* 0x54 */	&push<op_r_v>, &push<op_r_v>, &push<op_r_v>, &push<op_r_v>,
/* 0x58 */	inv, inv, inv, inv,
/* 0x5c */	inv, inv, inv, inv,
/* 0x60 */	inv, inv, inv, inv,
/* 0x64 */	inv, inv, &op_prefix<2>, &op_prefix<3>,
/* 0x68 */	&push<op_imm_v>, inv, &push<op_imm_b>, inv,
/* 0x6c */	inv, inv, inv, inv,
/* 0x70 */	&jo<op_imm_b>, &jno<op_imm_b>, &jb<op_imm_b>, &jae<op_imm_b>,
/* 0x74 */	&je<op_imm_b>, &jne<op_imm_b>, &jbe<op_imm_b>, &ja<op_imm_b>,
/* 0x78 */	&js<op_imm_b>, &jns<op_imm_b>, &jp<op_imm_b>, &jnp<op_imm_b>,
/* 0x7c */	&jl<op_imm_b>, &jge<op_imm_b>, &jle<op_imm_b>, &jg<op_imm_b>,
/* 0x80 */	op_group({inv, inv, inv, inv, inv, &sub<op_rm_b, op_imm_b>, inv, inv}),
/* 0x81 */  op_group({inv, inv, inv, inv, inv, &sub<op_rm_v, op_imm_v>, inv, inv}),
/* 0x82 */  inv,
/* 0x83 */  op_group({inv, inv, inv, inv, inv, &sub<op_rm_v, op_imm_b>, inv, inv}),
/* 0x84 */	&test<op_rm_b, op_reg_b>, &test<op_rm_v, op_reg_v>, inv, inv,
/* 0x88 */	&mov<op_rm_b, op_reg_b>, &mov<op_rm_v, op_reg_v>, &mov<op_reg_b, op_rm_b>, &mov<op_reg_v, op_rm_v>, //mov_r2rm_b, mov_r2rm_v, mov_rm2r_b, mov_rm2r_v,
/* 0x8c */	inv, inv, inv, inv,
/* 0x90 */	inv, inv, inv, inv,
/* 0x94 */	inv, inv, inv, inv,
/* 0x98 */	inv, inv, inv, inv,
/* 0x9c */	inv, inv, inv, inv,
/* 0xa0 */	&mov<op_a_b, op_moffs_b>, &mov<op_a_v, op_moffs_v>, &mov<op_moffs_b, op_a_b>, &mov<op_moffs_v, op_a_v>, //mov_moffs2a_b, mov_moffs2a_v, mov_a2moffs_b, mov_a2moffs_v,
/* 0xa4 */	inv, inv, inv, inv,
/* 0xa8 */	&test<op_a_b, op_imm_b>, &test<op_a_v, op_imm_v>, inv, inv,
/* 0xac */	inv, inv, inv, inv,
/* 0xb0 */	&mov<op_r_b, op_imm_b>, &mov<op_r_b, op_imm_b>, &mov<op_r_b, op_imm_b>, &mov<op_r_b, op_imm_b>, //mov_i2r_b, mov_i2r_b, mov_i2r_b, mov_i2r_b,
/* 0xb4 */	&mov<op_r_b, op_imm_b>, &mov<op_r_b, op_imm_b>, &mov<op_r_b, op_imm_b>, &mov<op_r_b, op_imm_b>, //mov_i2r_b, mov_i2r_b, mov_i2r_b, mov_i2r_b,
/* 0xb8 */	&mov<op_r_v, op_imm_v>, &mov<op_r_v, op_imm_v>, &mov<op_r_v, op_imm_v>, &mov<op_r_v, op_imm_v>, //mov_i2r_v, mov_i2r_v, mov_i2r_v, mov_i2r_v,
/* 0xbc */	&mov<op_r_v, op_imm_v>, &mov<op_r_v, op_imm_v>, &mov<op_r_v, op_imm_v>, &mov<op_r_v, op_imm_v>, //mov_i2r_v, mov_i2r_v, mov_i2r_v, mov_i2r_v,
/* 0xc0 */	op_group({inv, inv, inv, inv, inv, inv, inv, inv}),
/* 0xc1 */  op_group({inv, inv, inv, inv, inv, inv, inv, inv}),
/* 0xc2 */  inv, inv,
/* 0xc4 */	inv, inv, &mov<op_rm_b, op_imm_b>, &mov<op_rm_v, op_imm_v>, //mov_i2rm_b, mov_i2rm_v,
/* 0xc8 */	inv, inv, inv, inv,
/* 0xcc */	inv, inv, inv, inv,
/* 0xd0 */	op_group({inv, inv, inv, inv, inv, inv, inv, inv}),
/* 0xd1 */  op_group({inv, inv, inv, inv, inv, inv, inv, inv}),
/* 0xd2 */  op_group({inv, inv, inv, inv, inv, inv, inv, inv}),
/* 0xd3 */  op_group({inv, inv, inv, inv, inv, inv, inv, inv}),
/* 0xd4 */	inv, inv, nemu_trap, inv,
/* 0xd8 */	inv, inv, inv, inv,
/* 0xdc */	inv, inv, inv, inv,
/* 0xe0 */	inv, inv, inv, &jcxz<op_imm_b>,
/* 0xe4 */	inv, inv, inv, inv,
/* 0xe8 */	&call_near<op_imm_v>, inv, inv, inv,
/* 0xec */	inv, inv, inv, inv,
/* 0xf0 */	inv, inv, inv, inv,
/* 0xf4 */	inv, inv,
/* 0xf6 */  op_group({&test<op_rm_b, op_imm_b>, inv, inv, inv, inv, inv, inv, inv}),
/* 0xf7 */  op_group({&test<op_rm_v, op_imm_v>, inv, inv, inv, inv, inv, inv, inv}),
/* 0xf8 */	inv, inv, inv, inv,
/* 0xfc */	inv, inv,
/* 0xfe */  op_group({inv, inv, inv, inv, inv, inv, inv, inv}),
/* 0xff */  op_group({inv, inv, &call_near<op_rm_v>, inv, inv, inv, &push<op_rm_v>, inv})
};

helper_fun _2byte_opcode_table[256] = {
/* 0x00 */	op_group({inv, inv, inv, inv, inv, inv, inv, inv}),
/* 0x01 */  op_group({inv, inv, inv, inv, inv, inv, inv, inv}),
/* 0x02 */  inv, inv,
/* 0x04 */	inv, inv, inv, inv,
/* 0x08 */	inv, inv, inv, inv,
/* 0x0c */	inv, inv, inv, inv,
/* 0x10 */	inv, inv, inv, inv,
/* 0x14 */	inv, inv, inv, inv,
/* 0x18 */	inv, inv, inv, inv,
/* 0x1c */	inv, inv, inv, inv,
/* 0x20 */	inv, inv, inv, inv,
/* 0x24 */	inv, inv, inv, inv,
/* 0x28 */	inv, inv, inv, inv,
/* 0x2c */	inv, inv, inv, inv,
/* 0x30 */	inv, inv, inv, inv,
/* 0x34 */	inv, inv, inv, inv,
/* 0x38 */	inv, inv, inv, inv,
/* 0x3c */	inv, inv, inv, inv,
/* 0x40 */	inv, inv, inv, inv,
/* 0x44 */	inv, inv, inv, inv,
/* 0x48 */	inv, inv, inv, inv,
/* 0x4c */	inv, inv, inv, inv,
/* 0x50 */	inv, inv, inv, inv,
/* 0x54 */	inv, inv, inv, inv,
/* 0x58 */	inv, inv, inv, inv,
/* 0x5c */	inv, inv, inv, inv,
/* 0x60 */	inv, inv, inv, inv,
/* 0x64 */	inv, inv, inv, inv,
/* 0x68 */	inv, inv, inv, inv,
/* 0x6c */	inv, inv, inv, inv,
/* 0x70 */	inv, inv, inv, inv,
/* 0x74 */	inv, inv, inv, inv,
/* 0x78 */	inv, inv, inv, inv,
/* 0x7c */	inv, inv, inv, inv,
/* 0x80 */	&jo<op_imm_v>, &jno<op_imm_v>, &jb<op_imm_v>, &jae<op_imm_v>,
/* 0x84 */	&je<op_imm_v>, &jne<op_imm_v>, &jbe<op_imm_v>, &ja<op_imm_v>,
/* 0x88 */	&js<op_imm_v>, &jns<op_imm_v>, &jp<op_imm_v>, &jnp<op_imm_v>,
/* 0x8c */	&jl<op_imm_v>, &jge<op_imm_v>, &jle<op_imm_v>, &jg<op_imm_v>,
/* 0x90 */	inv, inv, inv, inv,
/* 0x94 */	inv, inv, inv, inv,
/* 0x98 */	inv, inv, inv, inv,
/* 0x9c */	inv, inv, inv, inv,
/* 0xa0 */	inv, inv, inv, inv,
/* 0xa4 */	inv, inv, inv, inv,
/* 0xa8 */	inv, inv, inv, inv,
/* 0xac */	inv, inv, inv, inv,
/* 0xb0 */	inv, inv, inv, inv,
/* 0xb4 */	inv, inv, inv, inv,
/* 0xb8 */	inv, inv, inv, inv,
/* 0xbc */	inv, inv, inv, inv,
/* 0xc0 */	inv, inv, inv, inv,
/* 0xc4 */	inv, inv, inv, inv,
/* 0xc8 */	inv, inv, inv, inv,
/* 0xcc */	inv, inv, inv, inv,
/* 0xd0 */	inv, inv, inv, inv,
/* 0xd4 */	inv, inv, inv, inv,
/* 0xd8 */	inv, inv, inv, inv,
/* 0xdc */	inv, inv, inv, inv,
/* 0xe0 */	inv, inv, inv, inv,
/* 0xe4 */	inv, inv, inv, inv,
/* 0xe8 */	inv, inv, inv, inv,
/* 0xec */	inv, inv, inv, inv,
/* 0xf0 */	inv, inv, inv, inv,
/* 0xf4 */	inv, inv, inv, inv,
/* 0xf8 */	inv, inv, inv, inv,
/* 0xfc */	inv, inv, inv, inv
};

extern "C" int exec(swaddr_t eip) {
	InstructionContext ctx;
	uint8_t opcode = instr_fetch(eip, 1);
	return opcode_table[opcode](ctx, eip);
}

helper_fun op_group(std::vector<helper_fun> fun_list) {
	return [fun_list(std::move(fun_list))] HELPER_PARAM_LIST {
		ModR_M modrm;
		modrm.value = instr_fetch(eip + 1, 1);
		ctx.require_modrm = true;
		int ret = (fun_list.begin()[modrm.regop]) (ctx, eip);
		ctx.require_modrm = false;
		return ret;
	};
}

template <size_t prefix_group>
HELPER(op_prefix) {
	ctx.prefix[prefix_group] = instr_fetch(eip, 1);
	int len = opcode_table[instr_fetch(eip + 1, 1)] (ctx, eip + 1);
	ctx.prefix[prefix_group] = 0;
	return len + 1;
}

HELPER(op_escape) {
	uint8_t _2nd_opcode = instr_fetch(eip + 1, 1);
	return _2byte_opcode_table[_2nd_opcode](ctx, eip) + 1;
}

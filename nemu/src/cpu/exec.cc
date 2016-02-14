#include <vector>

#include "decode.tcc"
#include "all-instr.h"

/* +rb/+rw/+rd occupies 8 opcodes to represent registers */
#define op_r_rep(...) __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__, \
						__VA_ARGS__, __VA_ARGS__, __VA_ARGS__, __VA_ARGS__
/* jcc/setcc/cmovcc */
#define op_cc(name, ...) \
/* 0x?0 */	&name##o<__VA_ARGS__>, &name##no<__VA_ARGS__>, &name##b<__VA_ARGS__>, &name##ae<__VA_ARGS__>, \
/* 0x?4 */	&name##e<__VA_ARGS__>, &name##ne<__VA_ARGS__>, &name##be<__VA_ARGS__>, &name##a<__VA_ARGS__>, \
/* 0x?8 */	&name##s<__VA_ARGS__>, &name##ns<__VA_ARGS__>, &name##p<__VA_ARGS__>, &name##np<__VA_ARGS__>, \
/* 0x?c */	&name##l<__VA_ARGS__>, &name##ge<__VA_ARGS__>, &name##le<__VA_ARGS__>, &name##g<__VA_ARGS__>
/* opcode with reg/op */
helper_fun op_group(std::vector<helper_fun> fun_list);
/* 2-byte opcode */
HELPER(op_escape);
/* instruction prefix */
template <size_t prefix_group> HELPER(op_prefix);

/* TODO: Add more instructions!!! */

helper_fun opcode_table[256] = {
/* 0x00 */	&add<op_rm_b, op_reg_b>, &add<op_rm_v, op_reg_v>, &add<op_reg_b, op_rm_b>, &add<op_reg_v, op_rm_v>,
/* 0x04 */	&add<op_a_b, op_imm_b>, &add<op_a_v, op_imm_v>, inv, inv,
/* 0x08 */	&or_<op_rm_b, op_reg_b>, &or_<op_rm_v, op_reg_v>, &or_<op_reg_b, op_rm_b>, &or_<op_reg_v, op_rm_v>,
/* 0x0c */	&or_<op_a_b, op_imm_b>, &or_<op_a_v, op_imm_v>, inv, op_escape,
/* 0x10 */	&adc<op_rm_b, op_reg_b>, &adc<op_rm_v, op_reg_v>, &adc<op_reg_b, op_rm_b>, &adc<op_reg_v, op_rm_v>,
/* 0x14 */	&adc<op_a_b, op_imm_b>, &adc<op_a_v, op_imm_v>, inv, inv,
/* 0x18 */	&sbb<op_rm_b, op_reg_b>, &sbb<op_rm_v, op_reg_v>, &sbb<op_reg_b, op_rm_b>, &sbb<op_reg_v, op_rm_v>,
/* 0x1c */	&sbb<op_a_b, op_imm_b>, &sbb<op_a_v, op_imm_v>, inv, inv,
/* 0x20 */	&and_<op_rm_b, op_reg_b>, &and_<op_rm_v, op_reg_v>, &and_<op_reg_b, op_rm_b>, &and_<op_reg_v, op_rm_v>,
/* 0x24 */	&and_<op_a_b, op_imm_b>, &and_<op_a_v, op_imm_v>, inv, inv,
/* 0x28 */	&sub<op_rm_b, op_reg_b>, &sub<op_rm_v, op_reg_v>, &sub<op_reg_b, op_rm_b>, &sub<op_reg_v, op_rm_v>,
/* 0x2c */	&sub<op_a_b, op_imm_b>, &sub<op_a_v, op_imm_v>, inv, inv,
/* 0x30 */	&xor_<op_rm_b, op_reg_b>, &xor_<op_rm_v, op_reg_v>, &xor_<op_reg_b, op_rm_b>, &xor_<op_reg_v, op_rm_v>,
/* 0x34 */	&xor_<op_a_b, op_imm_b>, &xor_<op_a_v, op_imm_v>, inv, inv,
/* 0x38 */	&cmp<op_rm_b, op_reg_b>, &cmp<op_rm_v, op_reg_v>, &cmp<op_reg_b, op_rm_b>, &cmp<op_reg_v, op_rm_v>,
/* 0x3c */	&cmp<op_a_b, op_imm_b>, &cmp<op_a_v, op_imm_v>, inv, inv,
/* 0x40 */	op_r_rep(&inc<op_r_v>),
/* 0x48 */	op_r_rep(&dec<op_r_v>),
/* 0x50 */	op_r_rep(&push<op_r_v>),
/* 0x58 */	op_r_rep(&pop<op_r_v>),
/* 0x60 */	pusha, popa, inv, inv,
/* 0x64 */	inv, inv, &op_prefix<2>, &op_prefix<3>,
/* 0x68 */	&push<op_imm_v>, &imul<op_reg_v, op_rm_v, op_imm_v>, &push<op_imm_b>, &imul<op_reg_v, op_rm_v, op_imm_b>,
/* 0x6c */	inv, inv, inv, inv,
/* 0x70 */	op_cc(j, op_imm_b),
/* 0x80 */	op_group({&add<op_rm_b, op_imm_b>, &or_<op_rm_b, op_imm_b>, &adc<op_rm_b, op_imm_b>, &sbb<op_rm_b, op_imm_b>, &and_<op_rm_b, op_imm_b>, &sub<op_rm_b, op_imm_b>, &xor_<op_rm_b, op_imm_b>, &cmp<op_rm_b, op_imm_b>}),
/* 0x81 */  op_group({&add<op_rm_v, op_imm_v>, &or_<op_rm_v, op_imm_v>, &adc<op_rm_v, op_imm_v>, &sbb<op_rm_v, op_imm_v>, &and_<op_rm_v, op_imm_v>, &sub<op_rm_v, op_imm_v>, &xor_<op_rm_v, op_imm_v>, &cmp<op_rm_v, op_imm_v>}),
/* 0x82 */  inv,
/* 0x83 */  op_group({&add<op_rm_v, op_imm_b>, &or_<op_rm_v, op_imm_b>, &adc<op_rm_v, op_imm_b>, &sbb<op_rm_v, op_imm_b>, &and_<op_rm_v, op_imm_b>, &sub<op_rm_v, op_imm_b>, &xor_<op_rm_v, op_imm_b>, &cmp<op_rm_v, op_imm_b>}),
/* 0x84 */	&test<op_rm_b, op_reg_b>, &test<op_rm_v, op_reg_v>, inv, inv,
/* 0x88 */	&mov<op_rm_b, op_reg_b>, &mov<op_rm_v, op_reg_v>, &mov<op_reg_b, op_rm_b>, &mov<op_reg_v, op_rm_v>,
/* 0x8c */	&mov<op_rm_w, op_reg_seg>, &lea<op_reg_v, op_rm_l>, &mov<op_reg_seg, op_rm_w>,
/* 0x8f */	op_group({&pop<op_rm_v>, inv, inv, inv, inv, inv, inv, inv}),
/* 0x90 */	op_r_rep(&xchg<op_a_v, op_r_v>),
/* 0x98 */	cwtl, cltd, inv, inv,
/* 0x9c */	inv, inv, inv, inv,
/* 0xa0 */	&mov<op_a_b, op_moffs_b>, &mov<op_a_v, op_moffs_v>, &mov<op_moffs_b, op_a_b>, &mov<op_moffs_v, op_a_v>,
/* 0xa4 */	movs, movs, cmps, cmps,
/* 0xa8 */	&test<op_a_b, op_imm_b>, &test<op_a_v, op_imm_v>, &stos, &stos,
/* 0xac */	inv, inv, inv, inv,
/* 0xb0 */	op_r_rep(&mov<op_r_b, op_imm_b>),
/* 0xb8 */	op_r_rep(&mov<op_r_v, op_imm_v>),
/* 0xc0 */	op_group({inv, inv, inv, inv, &shl<op_rm_b, op_imm_b>, &shr<op_rm_b, op_imm_b>, inv, &sar<op_rm_b, op_imm_b>}),
/* 0xc1 */  op_group({inv, inv, inv, inv, &shl<op_rm_v, op_imm_b>, &shr<op_rm_v, op_imm_b>, inv, &sar<op_rm_v, op_imm_b>}),
/* 0xc2 */  &ret_near<op_imm_w>, &ret_near<>,
/* 0xc4 */	inv, inv, &mov<op_rm_b, op_imm_b>, &mov<op_rm_v, op_imm_v>, //mov_i2rm_b, mov_i2rm_v,
/* 0xc8 */	inv, leave, inv, inv,
/* 0xcc */	int3, &intr<op_imm_b>, inv, iret,
/* 0xd0 */	op_group({inv, inv, inv, inv, &shl<op_rm_b, op_1>, &shr<op_rm_b, op_1>, inv, &sar<op_rm_b, op_1>}),
/* 0xd1 */  op_group({inv, inv, inv, inv, &shl<op_rm_v, op_1>, &shr<op_rm_v, op_1>, inv, &sar<op_rm_v, op_1>}),
/* 0xd2 */  op_group({inv, inv, inv, inv, &shl<op_rm_b, op_c_b>, &shr<op_rm_b, op_c_b>, inv, &sar<op_rm_b, op_c_b>}),
/* 0xd3 */  op_group({inv, inv, inv, inv, &shl<op_rm_v, op_c_b>, &shr<op_rm_v, op_c_b>, inv, &sar<op_rm_v, op_c_b>}),
/* 0xd4 */	inv, inv, nemu_trap, inv,
/* 0xd8 */	inv, inv, inv, inv,
/* 0xdc */	inv, inv, inv, inv,
/* 0xe0 */	inv, inv, inv, &jcxz<op_imm_b>,
/* 0xe4 */	&in<op_a_b, op_imm_b>, &in<op_a_v, op_imm_b>, &out<op_imm_b, op_a_b>, &out<op_imm_b, op_a_b>,
/* 0xe8 */	&call_near<op_imm_v>, &jmp_near<op_imm_v>, &jmp_long<op_ptrwv>, &jmp_near<op_imm_b>,
/* 0xec */	&in<op_a_b, op_d_w>, &in<op_a_v, op_d_w>, &out<op_d_w, op_a_b>, &out<op_d_w, op_a_v>,
/* 0xf0 */	&op_prefix<0>, inv, &op_prefix<0>, &op_prefix<0>,
/* 0xf4 */	inv, inv,
/* 0xf6 */  op_group({&test<op_rm_b, op_imm_b>, inv, &not_<op_rm_b>, &neg<op_rm_b>, &mul<op_rm_b>, &imul<op_rm_b>, &div<op_rm_b>, &idiv<op_rm_b>}),
/* 0xf7 */  op_group({&test<op_rm_v, op_imm_v>, inv, &not_<op_rm_v>, &neg<op_rm_v>, &mul<op_rm_v>, &imul<op_rm_v>, &div<op_rm_v>, &idiv<op_rm_v>}),
/* 0xf8 */	inv, inv, cli, sti,
/* 0xfc */	cld, stdf,
/* 0xfe */  op_group({&inc<op_rm_b>, &dec<op_rm_b>, inv, inv, inv, inv, inv, inv}),
/* 0xff */  op_group({&inc<op_rm_v>, &dec<op_rm_v>, &call_near<op_rm_v>, inv, &jmp_near<op_rm_v>, inv, &push<op_rm_v>, inv})
};

helper_fun _2byte_opcode_table[256] = {
/* 0x00 */	op_group({inv, inv, inv, inv, inv, inv, inv, inv}),
/* 0x01 */  op_group({inv, inv, &lgdt<op_rm_v>, &lidt<op_rm_v>, inv, inv, inv, inv}),
/* 0x02 */  inv, inv,
/* 0x04 */	inv, inv, inv, inv,
/* 0x08 */	inv, inv, inv, inv,
/* 0x0c */	inv, inv, inv, inv,
/* 0x10 */	inv, inv, inv, inv,
/* 0x14 */	inv, inv, inv, inv,
/* 0x18 */	inv, inv, inv, inv,
/* 0x1c */	inv, inv, inv,
/* 0x1f */  op_group({&nop<op_rm_v>, inv, inv, inv, inv, inv, inv, inv}),
/* 0x20 */	&mov<op_rm_l, op_reg_cr>, inv, &mov<op_reg_cr, op_rm_l>, inv,
/* 0x24 */	inv, inv, inv, inv,
/* 0x28 */	inv, inv, inv, inv,
/* 0x2c */	inv, inv, inv, inv,
/* 0x30 */	inv, inv, inv, inv,
/* 0x34 */	inv, inv, inv, inv,
/* 0x38 */	inv, inv, inv, inv,
/* 0x3c */	inv, inv, inv, inv,
/* 0x40 */	op_cc(cmov, op_reg_v, op_rm_v),
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
/* 0x80 */	op_cc(j, op_imm_v),
/* 0x90 */	op_cc(set, op_rm_b),
/* 0xa0 */	inv, inv, inv, inv,
/* 0xa4 */	&shld<op_rm_v, op_reg_v, op_imm_b>, &shld<op_rm_v, op_reg_v, op_c_b>, inv, inv,
/* 0xa8 */	inv, inv, inv, inv,
/* 0xac */	&shrd<op_rm_v, op_reg_v, op_imm_b>, &shrd<op_rm_v, op_reg_v, op_c_b>, inv, &imul<op_reg_v, op_rm_v>,
/* 0xb0 */	inv, inv, inv, inv,
/* 0xb4 */	inv, inv, &movzx<op_reg_v, op_rm_b>, &movzx<op_reg_v, op_rm_w>,
/* 0xb8 */	inv, inv, inv, inv,
/* 0xbc */	inv, inv, &movsx<op_reg_v, op_rm_b>, &movsx<op_reg_v, op_rm_w>,
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
    ctx.starting_eip = eip;
	ctx.opcode = instr_fetch(eip, 1);
	return opcode_table[ctx.opcode](ctx, eip);
}

helper_fun op_group(std::vector<helper_fun> fun_list) {
	helper_fun *fun_list_array = new helper_fun[8];
	copy(fun_list.begin(), fun_list.end(), fun_list_array);
	return [fun_list_array] HELPER_PARAM_LIST {
		ModR_M modrm;
		modrm.value = instr_fetch(eip + 1, 1);
		ctx.require_modrm = true;
		int ret = fun_list_array[modrm.regop] (ctx, eip);
		ctx.require_modrm = false;
		return ret;
	};
}

template <size_t prefix_group>
HELPER(op_prefix) {
	ctx.prefix[prefix_group] = instr_fetch(eip, 1);
	cpu.eip++;
	ctx.opcode = instr_fetch(eip + 1, 1);
	int len = opcode_table[ctx.opcode] (ctx, eip + 1);
	ctx.prefix[prefix_group] = 0;
	return len + 1;
}

HELPER(op_escape) {
	eip = ++cpu.eip;
	uint8_t _2nd_opcode = instr_fetch(eip, 1);
	ctx.opcode = (ctx.opcode << 8) | _2nd_opcode;
	return _2byte_opcode_table[_2nd_opcode](ctx, eip) + 1;
}

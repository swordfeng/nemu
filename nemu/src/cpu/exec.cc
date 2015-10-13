extern "C" {
#include "cpu/decode/modrm.h"
}

#include <functional>

#include "cpu/helper.h"
#include "all-instr.h"

typedef std::function<int (swaddr_t)> helper_fun;

/* opcode with reg/op */
helper_fun op_group(initializer_list<helper_fun> fun_list);
/* 2-byte opcode */
int op_escape(swaddr_t eip);

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
/* 0x64 */	inv, inv, data_size, address_size,
/* 0x68 */	inv, inv, inv, inv,
/* 0x6c */	inv, inv, inv, inv,
/* 0x70 */	inv, inv, inv, inv,
/* 0x74 */	inv, inv, inv, inv,
/* 0x78 */	inv, inv, inv, inv,
/* 0x7c */	inv, inv, inv, inv,
/* 0x80 */	group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0x81 */  group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0x82 */  inv,
/* 0x83 */  group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0x84 */	inv, inv, inv, inv,
/* 0x88 */	mov_r2rm_b, mov_r2rm_v, mov_rm2r_b, mov_rm2r_v,
/* 0x8c */	inv, inv, inv, inv,
/* 0x90 */	inv, inv, inv, inv,
/* 0x94 */	inv, inv, inv, inv,
/* 0x98 */	inv, inv, inv, inv,
/* 0x9c */	inv, inv, inv, inv,
/* 0xa0 */	mov_moffs2a_b, mov_moffs2a_v, mov_a2moffs_b, mov_a2moffs_v,
/* 0xa4 */	inv, inv, inv, inv,
/* 0xa8 */	inv, inv, inv, inv,
/* 0xac */	inv, inv, inv, inv,
/* 0xb0 */	mov_i2r_b, mov_i2r_b, mov_i2r_b, mov_i2r_b,
/* 0xb4 */	mov_i2r_b, mov_i2r_b, mov_i2r_b, mov_i2r_b,
/* 0xb8 */	mov_i2r_v, mov_i2r_v, mov_i2r_v, mov_i2r_v,
/* 0xbc */	mov_i2r_v, mov_i2r_v, mov_i2r_v, mov_i2r_v,
/* 0xc0 */	group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0xc1 */  group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0xc2 */  inv, inv,
/* 0xc4 */	inv, inv, mov_i2rm_b, mov_i2rm_v,
/* 0xc8 */	inv, inv, inv, inv,
/* 0xcc */	int3, inv, inv, inv,
/* 0xd0 */	group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0xd1 */  group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0xd2 */  group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0xd3 */  group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0xd4 */	inv, inv, nemu_trap, inv,
/* 0xd8 */	inv, inv, inv, inv,
/* 0xdc */	inv, inv, inv, inv,
/* 0xe0 */	inv, inv, inv, inv,
/* 0xe4 */	inv, inv, inv, inv,
/* 0xe8 */	inv, inv, inv, inv,
/* 0xec */	inv, inv, inv, inv,
/* 0xf0 */	inv, inv, inv, inv,
/* 0xf4 */	inv, inv,
/* 0xf6 */  group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0xf7 */  group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0xf8 */	inv, inv, inv, inv,
/* 0xfc */	inv, inv,
/* 0xfe */  group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0xff */  group(inv, inv, inv, inv, inv, inv, inv, inv)
};

helper_fun _2byte_opcode_table[256] = {
/* 0x00 */	group(inv, inv, inv, inv, inv, inv, inv, inv),
/* 0x01 */  group(inv, inv, inv, inv, inv, inv, inv, inv),
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
/* 0x80 */	inv, inv, inv, inv,
/* 0x84 */	inv, inv, inv, inv,
/* 0x88 */	inv, inv, inv, inv,
/* 0x8c */	inv, inv, inv, inv,
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

extern "C"
int exec(swaddr_t eip) {
	ops_decoded.opcode = instr_fetch(eip, 1);
	return opcode_table[ ops_decoded.opcode ](eip);
}

helper_fun op_group(initializer_list<helper_fun> fun_list) {
	return [fun_list(std::move(fun_list))] (swaddr_t eip) {
		// decode ModR_M
		// return fun_list.begin()[ModR_M.regop] (eip);
	}
}

int op_escape(swaddr_t eip) {
	eip++;
	uint32_t opcode = instr_fetch(eip, 1);
	ops_decoded.opcode = opcode | 0x100;
	return _2byte_opcode_table[opcode](eip) + 1;
}

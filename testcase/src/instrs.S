#include "trap.h"

.globl start
start:
  mov $8000000, %esp
  mov $1, %eax
  cmp $1, %eax
  jne bad
  call arith_test
  call ctrl_trans_test
  //call data_trans_test
  HIT_GOOD_TRAP

data_trans_test:
  mov $0xf001, %ax
  mov $0xff, %dx
  cwtl
  cmp $0xffff, %dx
  jne bad
  mov $1, %eax
  cltd
  cmp $0, %edx
  jne bad
  ret

ctrl_trans_test:
  // jmp
  jmp jmp_test0
  jmp bad
jmp_test0:
  // jcc
  mov $3, %eax
  cmp $0, %eax
  jz bad
  js bad
  jc bad
  jo bad
  jp bad
  cmp $5, %eax
  jo bad
  jz bad
  jnc bad
  jns bad
  jnp bad
  cmp $0x80000000, %eax
  jno bad
  cmp %eax, %eax
  jnz bad
  // call
  call call_test0
call_ret_pos:
  jmp bad
  // ret
  lea ret_ret_pos, %ebx
  push %ebx
  jmp ret_test0
  jmp bad
  jmp bad
  jmp bad
ret_ret_pos:
  ret
call_test0:
  pop %eax
  lea call_ret_pos, %edx
  cmp %eax, %edx
  jne bad
  add $5, %eax
  jmp %eax
ret_test0:
  ret

arith_test:
  // add
  mov $0x12, %al
  add $0xef, %al
  call test_cf
  mov $2147420000, %ecx
  add $100000, %ecx
  call test_of
  mov $0xffff0000, %ecx
  add $0x10000, %ecx
  call test_zf
  // sub
  mov $0, %eax
  sub $1, %eax
  call test_cf
  mov $0x7fffffff, %eax
  sub $0xffffffff, %eax
  call test_of
  mov $0xffffffff, %eax
  sub $0xffffffff, %eax
  call test_zf
  mov $0, %edx
  sub $0xffffffff, %edx
  call test_pf
  // adc
  call set_cf
  mov $0xffffffff, %eax
  adc $0, %eax
  call test_cf
  mov $0, %eax
  cmp $0, %eax
  mov $0xffffffff, %eax
  adc $1, %eax
  call test_zf
  // sbb
  mov $0, %eax
  call set_cf
  sbb $0, %eax
  call test_cf
  cmp $0xffffffff, %eax
  jne bad
  // inc
  mov $0x7fff, %ax
  inc %ax
  call test_of
  // dec
  dec %ax
  call test_of
  cmp $0x7fff, %ax
  jne bad
  // mul
  mov $1010101, %eax
  mov $808808808, %ecx
  mul %ecx
  cmp $0xa664d688, %eax
  jne bad
  cmp $0x2e709, %edx
  jne bad
  // imul
  imul $-50, %edx, %ecx
  cmp $-9510850, %ecx
  jne bad
  imul $-50, %ecx
  cmp $475542500, %ecx
  jne bad
  // div
  mov $0, %edx
  mov %ecx, %eax
  mov $65, %ecx
  div %ecx
  cmp $7316038, %eax
  jne bad
  // idiv
  mov $0xffffec00, %edx
  mov $0x353523a7, %eax
  mov $12345678, %ecx
  idiv %ecx
  cmp $-8350329, %edx
  jne bad
  cmp $-1781136, %eax
  jne bad
  // neg
  mov $12345678, %eax
  lea -1(%eax), %ecx
  not %ecx
  neg %eax
  cmp %eax, %ecx
  jne bad
  ret

test_cf:
  push %eax
  setc %al
  test %al, %al
  jz bad
  pop %eax
  ret

test_of:
  push %eax
  seto %al
  test %al, %al
  jz bad
  pop %eax
  ret

test_zf:
  push %eax
  setz %al
  test %al, %al
  jz bad
  pop %eax
  ret

test_pf:
  push %eax
  setp %al
  test %al, %al
  jz bad
  pop %eax
  ret

set_cf:
  push %eax
  mov $0, %eax
  cmp $1, %eax
  pop %eax
  ret

set_of:
  push %eax
  mov $0x7fffffff, %eax
  cmp $0xffffffff, %eax
  pop %eax
  ret

bad:
  HIT_BAD_TRAP

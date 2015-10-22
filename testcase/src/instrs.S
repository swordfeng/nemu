#include "trap.h"

.globl start
start:
  mov $8000000, %esp
  mov $1, %eax
  cmp $1, %eax
  jne bad
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
  HIT_GOOD_TRAP

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

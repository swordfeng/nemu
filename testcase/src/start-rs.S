.globl start
.globl hit_bad_trap
.globl hit_good_trap
.globl fmod
.globl fmodf
start:
# Set up a stack for C code. After the kernel is 
# introduced, the following code will never be executed.
	movl $0, %ebp
	movl $(128 << 20), %esp
	subl $16, %esp		# keep the `bt' command happy
	call main			# never return
fmod:
fmodf:
hit_bad_trap:
	movl $1, %eax
.byte 0xd6
hit_good_trap:
	movl $0, %eax
.byte 0xd6

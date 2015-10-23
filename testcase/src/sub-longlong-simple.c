#include "trap.h"

int main() {
	nemu_assert(0LL - 1LL == 0xffffffffffffffffULL);
	HIT_GOOD_TRAP;
	return 0;
}

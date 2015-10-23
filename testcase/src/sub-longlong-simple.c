#include "trap.h"

int main() {
	nemu_assert((long long)0 - (long long)1 == 0xffffffffffffffffULL);
	HIT_GOOD_TRAP;
	return 0;
}

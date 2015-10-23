#include "trap.h"

int main() {
	long long a = 0;
	long long b = 1;
	nemu_assert(a - b == 0xffffffffffffffffULL);
	HIT_GOOD_TRAP;
	return 0;
}

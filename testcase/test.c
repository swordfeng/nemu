#include "trap.h"

char str[] = "abcdefg";
int main() {
	char *s = str;
	HIT_GOOD_TRAP;
	return 0;
}

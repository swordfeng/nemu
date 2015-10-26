#include "trap.h"

extern char str[];
char str[] = "abcdefg";
int main() {
	HIT_GOOD_TRAP;
	return 0;
}

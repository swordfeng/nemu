#include "trap.h"

int t = 3;
char str[] = "abcdefg";
int main() {
	char *s = str;
	HIT_GOOD_TRAP;
	return 0;
}

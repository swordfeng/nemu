#include "trap.h"
#include "inline.h"

unsigned char a[] = {1,3,5,7,9,8,7,6};
unsigned char b[8];

int main() {
    for (int t = 4; t <= 8; t++) {
        fast_memcpy(b, a, t);
        for (int i = 0; i < t; i++) {
            if (a[i] != b[i]) {
                HIT_BAD_TRAP;
            }
        }
    }
    HIT_GOOD_TRAP;
}

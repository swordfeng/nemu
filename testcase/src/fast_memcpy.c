#include "trap.h"
#include "inline.h"

unsigned char a[1000];
unsigned char b[1000];

int main() {
    for (int i = 0; i < 1000; i++) a[i] = i;
    for (int i = 0; i < 1000; i++) b[i] = 0;
    for (int t = 0; t < 1000; t++) {
        for (int i = 0; i < t; i++) a[i]++;
        fast_memcpy(b, a, t + 1);
        for (int i = 0; i < t; i++) {
            if (a[i] != b[i]) {
                HIT_BAD_TRAP;
            }
        }
    }
    HIT_GOOD_TRAP;
}

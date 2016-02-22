#pragma once

inline static void *fast_memcpy(void *volatile dest, const void *volatile src, unsigned int n) {
    int dummy1, dummy2, dummy3;
    asm volatile("rep movsl;"
        "movl %4,%%ecx;"
        "rep movsb;"
        : "=D"(dummy1), "=S"(dummy2), "=c"(dummy3)
        : "c"(n >> 2), "g"(n & 3), "D"(dest), "S"(src)
        : "memory"
       );
    return dest;
}


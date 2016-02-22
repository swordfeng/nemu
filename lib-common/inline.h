#pragma once

inline static void *fast_memcpy(void *dest, const void *src, unsigned int n) {
    asm volatile("rep movsl;"
                 "mov %%eax,%%ecx;"
                 "rep movsb;": : "a"(n & 3), "c"(n >> 2), "S"(src), "D"(dest));
    return dest;
}


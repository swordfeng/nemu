#include <string.h>
#include <inttypes.h>
#include "avatar.h"
#include "trap.h"

void *video_mem = (void *) 0xa0000;

void outp(uint16_t port, uint8_t value) {
    asm volatile("outb %%al, %%dx" :: "a"(value), "d"(port));
}

int main() {
    for (uint32_t i = 0; i < 256; i++) {
        outp(0x3c8, i);
        outp(0x3c9, header_data_cmap[i][0] >> 2);
        outp(0x3c9, header_data_cmap[i][1] >> 2);
        outp(0x3c9, header_data_cmap[i][2] >> 2);
    }
    memcpy(video_mem, header_data, width * height);
    HIT_GOOD_TRAP;
}


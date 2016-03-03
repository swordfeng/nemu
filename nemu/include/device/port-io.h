#ifndef __PORT_IO_H__
#define __PORT_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"

typedef void(*pio_callback_t)(ioaddr_t, size_t, bool);

void* add_pio_map(ioaddr_t, size_t, pio_callback_t);

uint32_t pio_read(ioaddr_t, size_t);
void pio_write(ioaddr_t, size_t, uint32_t);

#ifdef __cplusplus
}
#endif

#endif

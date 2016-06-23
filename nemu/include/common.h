#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef DEEP_PERFORMANCE
#define PERFORMANCE
#endif

#ifndef PERFORMANCE
#    define USE_CACHE
#    define USE_TLB
#else
#    ifdef DEBUG
#        undef DEBUG
#    endif
#    define USE_TLB
#endif

#ifndef __GLASGOW_HASKELL__
//#define USE_RAMDISK

#define HAS_DEVICE


#include "debug.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>

#ifndef __cplusplus
typedef enum {
    false = 0,
    true = 1
} bool;
#endif

typedef uint32_t hwaddr_t;
typedef uint32_t lnaddr_t;
typedef uint32_t swaddr_t;

typedef uint16_t ioaddr_t;

#pragma pack (1)
typedef union {
    uint32_t _4;
    uint32_t _3    : 24;
    uint16_t _2;
    uint8_t _1;
} unalign;
#pragma pack ()
#define unalign_rw(addr, len)    (((unalign *)(addr))->_##len)
#endif

#endif

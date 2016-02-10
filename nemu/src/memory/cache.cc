#include <cinttypes>
#include <functional>
#include <iostream>
#include <iomanip>
#include "common.h"

#ifdef USE_CACHE

extern "C" {
    uint32_t dram_read(hwaddr_t addr, size_t len);
    void dram_write(hwaddr_t addr, size_t len, uint32_t data);
}

using std::function;

class CacheLine {
    public:
        uint32_t tag;
        uint8_t data[64];
        bool valid = false, dirty = false;
};

enum CacheWriteHitStrategy {
    WRITE_THROUGH,
    WRITE_BACK
};

enum CacheWriteMissStrategy {
    WRITE_ALLOCATE,
    WRITE_NOT_ALLOCATE
};

template<size_t indexes, size_t ways, CacheWriteHitStrategy hitStrategy, CacheWriteMissStrategy missStrategy>
class Cache {
    public:
        uint32_t read(hwaddr_t addr, size_t len);
        void write(hwaddr_t addr, size_t len, uint32_t data);
        void show(hwaddr_t addr);
        Cache(function<uint32_t (hwaddr_t, size_t)> r_fb, function<void (hwaddr_t, size_t, uint32_t)> w_fb): read_fallback(r_fb), write_fallback(w_fb) {}
    private:
        CacheLine cache[indexes][ways];
        function<uint32_t (hwaddr_t, size_t)> read_fallback;
        function<void (hwaddr_t, size_t, uint32_t)> write_fallback;

        CacheLine *getCacheLine(hwaddr_t addr) {
            uint32_t index = (addr >> 6) & (indexes - 1);
            uint32_t tag = (addr >> 6) / indexes;
            for (CacheLine *c = cache[index]; c < cache[index + 1]; c++) {
                if (c->valid && c->tag == tag) {
                    return c;
                }
            }
            return nullptr;
        }
        CacheLine *loadCacheLine(hwaddr_t addr) {
            uint32_t tag = (addr >> 6) / indexes;
            uint32_t index = (addr >> 6) & (indexes - 1);
            CacheLine *validline = getCacheLine(addr);
            if (!validline) {
                validline = &cache[index][rand() & (ways - 1)];
                hwaddr_t start_addr = addr & ~0x3F;
                if (validline->valid && validline->dirty) {
                    for (uint32_t i = 0; i < 64; i += 4) write_fallback(start_addr + i, 4, unalign_rw(validline->data + i, 4));
                }
                // read to line
                validline->valid = true;
                validline->dirty = false;
                validline->tag = tag;
                for (uint32_t i = 0; i < 64; i += 4) unalign_rw(validline->data + i, 4) = read_fallback(start_addr + i, 4);
            }
            return validline;
        }
};

template<size_t indexes, size_t ways, CacheWriteHitStrategy hitStrategy, CacheWriteMissStrategy missStrategy>
uint32_t Cache<indexes, ways, hitStrategy, missStrategy>::read(hwaddr_t addr, size_t len) {
    uint32_t max_len = 0x40 - (addr & 0x3F);
    if (max_len < len) {
        // cross boundary
        uint32_t low = read(addr, max_len);
        uint32_t high = read(addr + max_len, len - max_len);
        return (high << (max_len * 8)) | low;
    }

    CacheLine *l = loadCacheLine(addr);
    uint32_t block_offset = addr & 0x3F;
    return unalign_rw(l->data + block_offset, 4) & ((1llu << (len * 8)) - 1);
}

template<size_t indexes, size_t ways, CacheWriteHitStrategy hitStrategy, CacheWriteMissStrategy missStrategy>
inline void Cache<indexes, ways, hitStrategy, missStrategy>::write(hwaddr_t addr, size_t len, uint32_t data) {
    uint32_t max_len = 0x40 - (addr & 0x3F);
    if (max_len < len) {
        // cross boundary
        write(addr, max_len, data & ((1 << (max_len * 8)) - 1));
        write(addr + max_len, len - max_len, data >> (max_len * 8));
        return;
    }
    CacheLine *l = this->getCacheLine(addr);
    if (l != nullptr) {
        // write hit
        uint32_t block_offset = addr & 0x3F;
        uint32_t temp_data = data;
        for (uint32_t i = 0; i < len; i++) {
            l->data[block_offset + i] = temp_data & 0xff;
            temp_data >>= 8;
        }
        if (hitStrategy == WRITE_THROUGH) {
            write_fallback(addr, len, data);
        } else if (hitStrategy == WRITE_BACK) {
            l->dirty = true;
        }
    } else {
        // write miss
        if (missStrategy == WRITE_ALLOCATE) {
            loadCacheLine(addr);
            write(addr, len, data);
        } else if (missStrategy == WRITE_NOT_ALLOCATE) {
            write_fallback(addr, len, data);
        }
    }
}

template<size_t indexes, size_t ways, CacheWriteHitStrategy hitStrategy, CacheWriteMissStrategy missStrategy>
inline void Cache<indexes, ways, hitStrategy, missStrategy>::show(hwaddr_t addr) {
    CacheLine *l = getCacheLine(addr);
    if (l == nullptr) {
        std::cout << "miss" << std::endl;
    } else {
        std::cout << "hit, tag = " << std::hex << l->tag << ", valid = " << l->valid << ", dirty = " << l->dirty << std::endl;
        for (int i = 0; i < 64; i += 8) {
            for (int j = 0; j < 8; j++) {
                std::cout << std::setfill('0') << std::setw(2) << std::hex << (uint32_t) l->data[i + j] << " ";
            }
            std::cout << std::endl;
        }
    }
}

using namespace std::placeholders;
static Cache<4096, 16, WRITE_BACK, WRITE_ALLOCATE> L2(dram_read, dram_write);
static Cache<128, 8, WRITE_THROUGH, WRITE_NOT_ALLOCATE> L1(std::bind(&decltype(L2)::read, &L2, _1, _2), std::bind(&decltype(L2)::write, &L2, _1, _2, _3));

extern "C" {
    uint32_t cached_read(hwaddr_t addr, size_t len) {
        return L1.read(addr, len);
    }
    void cached_write(hwaddr_t addr, size_t len, uint32_t data) {
        L1.write(addr, len, data);
    }
    void cache_show(hwaddr_t addr) {
        std::cout << "L1 cache: ";
        L1.show(addr);
        std::cout << "L2 cache: ";
        L2.show(addr);
        std::cout << std::flush;
    }
}

#endif

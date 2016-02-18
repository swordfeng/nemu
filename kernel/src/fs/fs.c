#include "common.h"
#include <string.h>

typedef struct {
    char *name;
    uint32_t size;
    uint32_t disk_offset;
} file_info;

enum {SEEK_SET, SEEK_CUR, SEEK_END};

/* This is the information about all files in disk. */
static const file_info file_table[] = {
    {"1.rpg", 188864, 1048576}, {"2.rpg", 188864, 1237440},
    {"3.rpg", 188864, 1426304}, {"4.rpg", 188864, 1615168},
    {"5.rpg", 188864, 1804032}, {"abc.mkf", 1022564, 1992896},
    {"ball.mkf", 134704, 3015460}, {"data.mkf", 66418, 3150164},
    {"desc.dat", 16027, 3216582}, {"fbp.mkf", 1128064, 3232609},
    {"fire.mkf", 834728, 4360673}, {"f.mkf", 186966, 5195401},
    {"gop.mkf", 11530322, 5382367}, {"map.mkf", 1496578, 16912689},
    {"mgo.mkf", 1577442, 18409267}, {"m.msg", 188232, 19986709},
    {"mus.mkf", 331284, 20174941}, {"pat.mkf", 8488, 20506225},
    {"rgm.mkf", 453202, 20514713}, {"rng.mkf", 4546074, 20967915},
    {"sss.mkf", 557004, 25513989}, {"voc.mkf", 1997044, 26070993},
    {"wor16.asc", 5374, 28068037}, {"wor16.fon", 82306, 28073411},
    {"word.dat", 5650, 28155717},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);

/* TODO: implement a simplified file system here. */

struct {
    bool opened;
    uint32_t offset;
} Fstate[NR_FILES + 3];

int fs_open(const char *pathname, int flags) {
    for (int i = 0; i < NR_FILES; i++) {
        if (strcmp(file_table[i].name, pathname) == 0) {
            int fd = i + 3;
            Fstate[fd].opened = true;
            Fstate[fd].offset = 0;
            return fd;
        }
    }
    panic("file not found");
    return -1;
}

int fs_read(int fd, void *buf, int len) {
    if (fd < 3) return -1;
    assert(fd < NR_FILES + 3);
    assert(Fstate[fd].opened);
    int max_len = file_table[fd - 3].size - Fstate[fd].offset;
    if (max_len < len) len = max_len;
    ide_read(buf, file_table[fd - 3].disk_offset + Fstate[fd].offset, len);
    return len;
}

#include "nemu.h"

#define ENTRY_START 0x100000

extern uint8_t entry [];
extern uint32_t entry_len;
extern char *exec_file;
extern unsigned char init_logo[];

void load_elf_tables(int, char *[]);
void init_regex();
void init_wp_list();
void init_ddr3();

void init_device();
void init_sdl();

FILE *log_fp = NULL;

#ifdef LOG_FILE
static void init_log() {
    log_fp = fopen(LOG_FILE, "w");
    Assert(log_fp, "Can not open 'log.txt'");
}
#endif

static void welcome() {
    printf("Welcome to NEMU!\n%sThe executable is %s.\nFor help, type \"help\"\n",
            init_logo, exec_file);
}

void init_monitor(int argc, char *argv[]) {
    /* Perform some global initialization */

    /* Open the log file. */
#ifdef LOG_FILE
    init_log();
#endif

    /* Load the string table and symbol table from the ELF file for future use. */
    load_elf_tables(argc, argv);

#ifndef PERFORMANCE
    /* Initialize the watchpoint link list. */
    init_wp_list();
#endif

    /* Display welcome message. */
    welcome();

#ifdef HAS_DEVICE
    init_device();
    init_sdl();
#endif
}

#ifdef USE_RAMDISK
static void init_ramdisk() {
    int ret;
    const int ramdisk_max_size = 0xa0000;
    FILE *fp = fopen(exec_file, "rb");
    Assert(fp, "Can not open '%s'", exec_file);

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    Assert(file_size < ramdisk_max_size, "file size(%zd) too large", file_size);

    fseek(fp, 0, SEEK_SET);
    ret = fread(hwa_to_va(0), file_size, 1, fp);
    assert(ret == 1);
    fclose(fp);
}
#endif

static void load_entry() {
    int ret;
    FILE *fp = fopen("entry", "rb");
    Assert(fp, "Can not open 'entry'");

    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);

    fseek(fp, 0, SEEK_SET);
    ret = fread(hwa_to_va(ENTRY_START), file_size, 1, fp);
    assert(ret == 1);
    fclose(fp);
}

void restart() {
    /* Perform some initialization to restart a program */
#ifdef USE_RAMDISK
    /* Read the file with name `argv[1]' into ramdisk. */
    init_ramdisk();
#endif

    /* Read the entry code into memory. */
    load_entry();

    /* Set initial register values */
    init_reg();

    /* Set the initial instruction pointer. */
    cpu.eip = ENTRY_START;

    /* Initialize DRAM. */
    init_ddr3();

#ifdef USE_TLB
    void init_tlb();
    init_tlb();
#endif

}

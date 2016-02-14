##### global settings #####

.PHONY: nemu entry all_testcase kernel run gdb test submit clean

MAKEFLAGS := -j4

CC := gcc
CXX := g++
RUSTC := rustc
HC := ghc
LD := ld
ASFLAGS := -m32 -MMD -c
CFLAGS := -MMD -Wall -Werror -c
CXXFLAGS := -MMD -Wall -Werror -c -std=c++14 -fno-exceptions
HCFLAGS := -c
RSFLAGS := --emit obj

LIB_COMMON_DIR := lib-common
NEWLIBC_DIR := $(LIB_COMMON_DIR)/newlib
NEWLIBC := $(NEWLIBC_DIR)/libc.a
FLOAT := obj/$(LIB_COMMON_DIR)/FLOAT.a
RSCORE := $(LIB_COMMON_DIR)/libcore.a

include config/Makefile.git
include config/Makefile.build

all: nemu


##### rules for building the project #####

include nemu/Makefile.part
include testcase/Makefile.part
include lib-common/Makefile.part
include kernel/Makefile.part
include game/Makefile.part

nemu: $(nemu_BIN)
all_testcase: $(testcase_BIN)
kernel: $(kernel_BIN)
game: $(game_BIN)


##### rules for cleaning the project #####

clean-nemu:
	-rm -rf obj/nemu 2> /dev/null

clean-testcase:
	-rm -rf obj/testcase 2> /dev/null

clean-kernel:
	-rm -rf obj/kernel 2> /dev/null

clean-game:
	-rm -rf obj/game 2> /dev/null

clean-log:
	-rm -f *log.txt entry $(FLOAT) 2> /dev/null

clean: clean-cpp clean-log
	-rm -rf obj 2> /dev/null

##### some convinient rules #####
TEST=hello-inline-asm
USERPROG := obj/testcase/$(TEST)
ENTRY := $(kernel_BIN)
#ENTRY := $(USERPROG)

entry: $(ENTRY)
	objcopy -S -O binary $(ENTRY) entry

run: $(nemu_BIN) $(USERPROG) entry
	$(call git_commit, "run")
	$(nemu_BIN) $(USERPROG)

gdb: $(nemu_BIN) $(USERPROG) entry
	gdb -s $(nemu_BIN) --args $(nemu_BIN) $(USERPROG)

test: $(nemu_BIN) $(testcase_BIN) entry
	bash test.sh $(testcase_BIN)

submit: clean
	cd .. && tar cvj $(shell pwd | grep -o '[^/]*$$') > $(STU_ID).tar.bz2

count-nemu:
	find nemu/ -regextype posix-egrep -regex ".*\.c|.*\.h|.*\.hc|.*\.hs" -exec cat '{}' \; | sed '/^\s*$$/d' | wc -l

profile:
	@gprof obj/nemu/nemu > prof.txt
	@echo "See prof.txt"

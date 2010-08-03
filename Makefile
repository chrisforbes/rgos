.SUFFIXES:
.PHONY: clean all
.DEFAULT_GOAL: all

CFLAGS = -Wall -Wextra -Werror -nostartfiles -nodefaultlibs -nostdlib
S_SRCS := $(shell find src/ -iname '*.s')
C_SRCS := $(shell find src/ -iname '*.c')

%.o: %.c
	@echo CC $<
	@gcc -o $@ -c $< $(CFLAGS)

%.o: %.s
	@echo AS $<
	@as -o $@ $<

kernel.elf: $(S_SRCS:.s=.o) $(C_SRCS:.c=.o)
	@echo LD $@
	@ld -T src/kernel.ld -o $@ $^

all: rgos.iso

rgos.iso: kernel.elf

# Simple RISC-V 32-bit bare-metal OS Makefile

# Use the toolchain you actually have installed
CROSS_PREFIX ?= riscv64-unknown-elf-

CC      := $(CROSS_PREFIX)gcc
LD      := $(CROSS_PREFIX)gcc
OBJCOPY := $(CROSS_PREFIX)objcopy

# These flags FORCE 32-bit output, even though gcc prefix is riscv64
CFLAGS  := -march=rv32imac -mabi=ilp32 \
           -ffreestanding -nostdlib -nostartfiles \
           -Wall -Wextra -O2 \
           -Ikernel -Iuser

LDFLAGS := -T linker.ld -nostdlib -ffreestanding

KERNEL  := kernel.elf

KERNEL_SRCS := \
    kernel/kmain.c \
    kernel/uart.c \
    kernel/common.c \
    kernel/sync.c \
    kernel/fs.c \
    kernel/sched.c \
    user/user_programs.c

ASM_SRCS := boot/start.S kernel/switch.S

OBJS := $(KERNEL_SRCS:.c=.o) $(ASM_SRCS:.S=.o)

.PHONY: all clean run

all: $(KERNEL)

$(KERNEL): $(OBJS) linker.ld
	$(LD) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(KERNEL)

# Run on QEMU virt, 32-bit, no BIOS, UART on stdio
run: $(KERNEL)
	qemu-system-riscv32 -machine virt -nographic \
	    -bios none -kernel $(KERNEL)

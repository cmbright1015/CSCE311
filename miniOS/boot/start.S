    .section .text.entry
    .globl _start

_start:
    # Set stack pointer
    la   sp, _stack_top

    # Zero .bss
    la   a0, __bss_start
    la   a1, __bss_end
1:
    beq  a0, a1, 2f
    sw   x0, 0(a0)
    addi a0, a0, 4
    j    1b
2:
    # Call C kernel main
    call kmain

# If kmain returns, just halt in a loop.
hang:
    wfi
    j hang

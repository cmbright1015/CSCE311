/* boot/crt.S - riscv64 entry called by OpenSBI (or QEMU -kernel with bios) */
/* a0 = hartid, a1 = fdt/dtb pointer */
    .section .text
    .global _start
    .option norvc
_start:
    /* Set up a minimal stack pointer. Linker will place stack_top label. */
    la sp, _stack_top
    /* call kernel_main(hartid, dtb) */
    /* a0 and a1 already hold hartid and dtb (per OpenSBI convention) */
    call kernel_main
1:  j 1b

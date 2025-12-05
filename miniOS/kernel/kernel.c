/* kernel/kernel.c */
#include <stdint.h>
#include "console.h"

/* kernel_main is called from crt.S with:
   a0 = hartid, a1 = dtb pointer */
void kernel_main(uintptr_t hartid, void *dtb) {
    (void)dtb;
    console_puts("miniOS RISCV: kernel_main()\n");
    console_puts("Hart: ");
    /* very simple hart id print (decimal) */
    char buf[32];
    int n = 0;
    uintptr_t h = hartid;
    if (h == 0) {
        console_putc('0');
    } else {
        char tmp[32];
        while (h) { tmp[n++] = '0' + (h % 10); h /= 10; }
        while (n--) console_putc(tmp[n]);
    }
    console_puts("\nBoot complete — halting.\n");
    for (;; ) { asm volatile("wfi"); } /* wait-for-interrupt low-power loop */
}

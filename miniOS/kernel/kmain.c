#include <stdint.h>
#include "uart.h"
#include "fs.h"
#include "sched.h"
#include "common.h"

/* User task entry points */
void user_hello(void);
void user_counter(void);

void kmain(void)
{
    uart_init();
    uart_puts("\n\nminiOS (RISC-V 32) booting...\n");

    fs_init();
    scheduler_init();

    /* Example file owned by "kernel" (id -1 -> public read) */
    int fd = fs_create("greeting.txt", -1, 1u | 2u);
    const char *msg = "Hello from miniOS kernel!\n";
    fs_write(fd, msg, kstrlen(msg));

    fs_list();

    /* Spawn user tasks */
    task_create(user_hello);
    task_create(user_counter);

    uart_puts("Starting scheduler...\n");
    scheduler_start();

    uart_puts("All tasks finished, back in kernel. Halting.\n");

    for (;;) {
        __asm__ volatile ("wfi");
    }
}

#include "uart.h"
#include "fs.h"
#include "sched.h"

void user_hello(void)
{
    int tid = current_task_id();
    uart_printf("[hello] task %d starting\n", tid);

    int fd = fs_open("greeting.txt", tid);
    if (fd >= 0) {
        char buf[64];
        int n = fs_read(fd, buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            uart_printf("[hello] read from file: %s", buf);
        } else {
            uart_puts("[hello] fs_read failed\n");
        }
    } else {
        uart_puts("[hello] fs_open failed\n");
    }

    uart_puts("[hello] yielding to other tasks\n");
    task_yield();
    uart_puts("[hello] done\n");
}

void user_counter(void)
{
    int tid = current_task_id();
    uart_printf("[counter] task %d starting\n", tid);

    for (int i = 0; i < 10; i++) {
        uart_printf("[counter] i = %d\n", i);
        task_yield();
    }

    uart_puts("[counter] done\n");
}

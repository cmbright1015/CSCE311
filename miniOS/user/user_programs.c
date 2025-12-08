#include "uart.h"
#include "fs.h"
#include "sched.h"


/* sipmle tasks to make sure its getting the task ID, opening and reading the file and multitasking using task_yield()*/
void user_hello(void)
{
    /* getID of current tasks*/
    int tid = current_task_id();
    uart_printf("[hello] task %d starting\n", tid);

    /* open the file*/
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

/* tasks used to test creation, schduling, multitasking and yielding, yields after each point 0-9*/
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

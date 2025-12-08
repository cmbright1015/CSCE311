#include "sched.h"
#include "uart.h"

static task_t tasks[MAX_TASKS];
static int    current = -1;
static context_t kernel_ctx;

static int pick_next_runnable(void);


/* initialize scheudler structures before any tasks are created, every tasks begins in known state and the schduler doesn't assume previous state memory*/
void scheduler_init(void)
{
    for (int i = 0; i < MAX_TASKS; i++) {
        tasks[i].id    = i;
        tasks[i].state = TASK_UNUSED;
        tasks[i].entry = 0;
    }
    current = -1;
}

/* Find unused slots for new tasks */
static int alloc_task(void)
{
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_UNUSED)
            return i;
    }
    return -1;
}

/* starting point for new tasks, we do this because its better for the scheduler to have a certain place to find new tasks */
static void task_trampoline(void);

/* creates new tasks, these tasks run entry() when they are scheduled
allocates free slot, marks the slot as ready, sets up ra and sp which is the address of task_trampoline and the top of the task stack*/
int task_create(task_entry_t entry)
{
    int idx = alloc_task();
    if (idx < 0)
        return -1;

    task_t *t = &tasks[idx];
    t->entry = entry;
    t->state = TASK_READY;

    /* New task context: separate stack, start at task_trampoline */
    for (int i = 0; i < (int)sizeof(context_t)/4; i++) {
        ((uint32_t *)&t->ctx)[i] = 0;
    }

    t->ctx.ra = (uint32_t)task_trampoline;
    t->ctx.sp = (uint32_t)(t->stack + STACK_SIZE);

    return idx;
}

/* picks tasks round robin style*/
static int pick_next_runnable(void)
{
    if (current < 0)
        current = 0;

    for (int n = 0; n < MAX_TASKS; n++) {
        int idx = (current + 1 + n) % MAX_TASKS;
        if (tasks[idx].state == TASK_READY)
            return idx;
    }
    return -1;
}
/* prints the ID of running tasks*/
int current_task_id(void)
{
    return current;
}
/* when a task is running it calls this to give up the CPU for other tasks*/
void task_yield(void)
{
    int prev = current;
    int next = pick_next_runnable();

    if (prev < 0) return; /* not started yet */

    if (next < 0) {
        /* No runnable tasks, go back to kernel */
        current = -1;
        context_switch(&tasks[prev].ctx, &kernel_ctx);
        return;
    }

    if (next == prev)
        return; /* only one runnable */

    tasks[prev].state = TASK_READY;
    tasks[next].state = TASK_RUNNING;

    current = next;
    context_switch(&tasks[prev].ctx, &tasks[next].ctx);
}


/* begin tasks */
void scheduler_start(void)
{
    int next = -1;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (tasks[i].state == TASK_READY) {
            next = i;
            break;
        }
    }

    if (next < 0) {
        uart_puts("scheduler_start: no tasks\n");
        return;
    }

    current = next;
    tasks[next].state = TASK_RUNNING;

    uart_puts("scheduler_start: switching to first task\n");
    context_switch(&kernel_ctx, &tasks[next].ctx);
}

/* all tasks start here after context switch, identifies current tasks, calls entry(), mark the state as FINISHED, try to give the CPU up then of no other tasks
switch back to the kernel*/
static void task_trampoline(void)
{
    int id = current;
    if (id < 0 || id >= MAX_TASKS)
        return;

    task_t *t = &tasks[id];
    if (t->entry)
        t->entry();

    t->state = TASK_FINISHED;
    uart_printf("task %d finished\n", id);

    /* Look for another runnable task or return to kernel */
    task_yield();

    /* If we reach here, no more tasks: switch back to kernel */
    context_switch(&t->ctx, &kernel_ctx);
}

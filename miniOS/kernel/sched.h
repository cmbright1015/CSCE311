#pragma once
#include <stdint.h>

typedef void (*task_entry_t)(void);

typedef enum {
    TASK_UNUSED = 0,
    TASK_READY,
    TASK_RUNNING,
    TASK_FINISHED
} task_state_t;

typedef struct context {
    uint32_t ra;
    uint32_t sp;
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
} context_t;

#define MAX_TASKS   4
#define STACK_SIZE  1024

typedef struct task {
    int          id;
    task_state_t state;
    context_t    ctx;
    task_entry_t entry;
    uint8_t      stack[STACK_SIZE];
} task_t;

void scheduler_init(void);
int  task_create(task_entry_t entry);
void scheduler_start(void);
void task_yield(void);
int  current_task_id(void);

/* Implemented in assembly */
void context_switch(context_t *old, context_t *new);

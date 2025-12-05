#pragma once
#include <stdint.h>

typedef struct {
    volatile uint32_t locked;
} spinlock_t;

void spinlock_init(spinlock_t *l);
void spinlock_lock(spinlock_t *l);
void spinlock_unlock(spinlock_t *l);

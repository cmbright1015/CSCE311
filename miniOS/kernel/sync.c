#include "sync.h"

void spinlock_init(spinlock_t *l)
{
    l->locked = 0;
}

void spinlock_lock(spinlock_t *l)
{
    /* Test-and-set spinlock */
    while (__sync_lock_test_and_set(&l->locked, 1)) {
        __asm__ volatile ("nop");
    }
}

void spinlock_unlock(spinlock_t *l)
{
    __sync_lock_release(&l->locked);
}

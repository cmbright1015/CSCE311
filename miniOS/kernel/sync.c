#include "sync.h"


/* the spinlock locks at one and that means a critical section is being used, we need this because some parts of the code may
be modifying data and others are trying to read/write to it */
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

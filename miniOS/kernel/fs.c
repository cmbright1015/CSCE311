#include "fs.h"
#include "common.h"
#include "uart.h"

static file_t files[MAX_FILES];
static spinlock_t fs_lock;


/* this initializes the file subsystem, we need every entry to be consistent and we also need the spinlock so multiple tasks can't corrupt the FS.*/
void fs_init(void)
{
    spinlock_init(&fs_lock);
    for (int i = 0; i < MAX_FILES; i++) {
        files[i].in_use = 0;
        files[i].owner  = -1;
        files[i].size   = 0;
    }
}

/* Find file index by name, or -1 */
static int fs_find(const char *name)
{
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].in_use &&
            kstrlen(name) == kstrlen(files[i].name)) {
            const char *a = name;
            const char *b = files[i].name;
            size_t j = 0;
            for (; j < kstrlen(name); j++) {
                if (a[j] != b[j]) break;
            }
            if (j == kstrlen(name))
                return i;
        }
    }
    return -1;
}

/* creates files with name, owner ID, and permissions, this needs the spinlock because without it two tasks could create files in the same slot.*/
int fs_create(const char *name, int owner, uint32_t perm)
{
    spinlock_lock(&fs_lock);

    int existing = fs_find(name);
    if (existing >= 0) {
        spinlock_unlock(&fs_lock);
        return existing;
    }

    /* find free slots in the table*/
    int idx = -1;
    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i].in_use) {
            idx = i;
            break;
        }
    }
    if (idx < 0) {
        spinlock_unlock(&fs_lock);
        return -1; /* no space */
    }

    /* initialize the files data copy the name and then return file descriptor*/
    file_t *f = &files[idx];
    size_t n = kstrlen(name);
    if (n >= MAX_FILE_NAME)
        n = MAX_FILE_NAME - 1;

    for (size_t i = 0; i < n; i++)
        f->name[i] = name[i];
    f->name[n] = '\0';

    f->in_use = 1;
    f->owner  = owner;
    f->perm   = perm;
    f->size   = 0;

    spinlock_unlock(&fs_lock);
    return idx;
}

/* Very small permission check */
static int fs_can_access(file_t *f, int requester, uint32_t needed)
{
    if (!f->in_use) return 0;
    if ((f->perm & needed) == 0) return 0;
    if (f->owner == -1) return 1;      /* public file */
    if (f->owner == requester) return 1;
    return 0;
}


/* this locates the file and makes sure the user has read access*/
int fs_open(const char *name, int requester)
{
    spinlock_lock(&fs_lock);

    int idx = fs_find(name);
    if (idx < 0) {
        spinlock_unlock(&fs_lock);
        return -1;
    }

    file_t *f = &files[idx];
    if (!fs_can_access(f, requester, 1u)) {
        spinlock_unlock(&fs_lock);
        return -1;
    }

    spinlock_unlock(&fs_lock);
    return idx;
}


/* spinlock is used here when writing files because if two tasks write at the same time corruption happens*/
int fs_write(int fd, const void *buf, size_t len)
{
    if (fd < 0 || fd >= MAX_FILES) return -1;

    spinlock_lock(&fs_lock);
    file_t *f = &files[fd];

    if (!fs_can_access(f, f->owner, 2u)) {
        spinlock_unlock(&fs_lock);
        return -1;
    }

    if (len > MAX_FILE_SIZE)
        len = MAX_FILE_SIZE;

    const uint8_t *src = (const uint8_t *)buf;
    for (size_t i = 0; i < len; i++)
        f->data[i] = src[i];

    f->size = len;

    spinlock_unlock(&fs_lock);
    return (int)len;
}

/* reads file data into buffer, locks it so you can't modify file data while reading.*/
int fs_read(int fd, void *buf, size_t len)
{
    if (fd < 0 || fd >= MAX_FILES) return -1;

    spinlock_lock(&fs_lock);
    file_t *f = &files[fd];

    if (!fs_can_access(f, -1, 1u)) {
        spinlock_unlock(&fs_lock);
        return -1;
    }

    if (len > f->size)
        len = f->size;

    uint8_t *dst = (uint8_t *)buf;
    for (size_t i = 0; i < len; i++)
        dst[i] = f->data[i];

    spinlock_unlock(&fs_lock);
    return (int)len;
}


/* prints existing files to the UART console, helps with debugging and seeing the FS state. kind of like "ls" in linux terminals.*/
void fs_list(void)
{
    spinlock_lock(&fs_lock);

    uart_puts("Files:\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i].in_use) {
            uart_printf("  %s (size=%d, owner=%d, perm=0x%x)\n",
                        files[i].name, (int)files[i].size,
                        files[i].owner, files[i].perm);
        }
    }

    spinlock_unlock(&fs_lock);
}

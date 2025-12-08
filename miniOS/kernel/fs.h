#pragma once
#include <stddef.h>
#include <stdint.h>

#include "sync.h"

#define MAX_FILES      8
#define MAX_FILE_NAME  16
#define MAX_FILE_SIZE  256

/* perm bits: 1 = read, 2 = write */
typedef struct {
    char name[MAX_FILE_NAME];
    uint8_t data[MAX_FILE_SIZE];
    size_t size;
    int in_use;
    int owner;       /* task id that owns this file, or -1 for public */
    uint32_t perm;   /* bitmask */
} file_t;

void fs_init(void);
int  fs_create(const char *name, int owner, uint32_t perm);
int  fs_open(const char *name, int requester);
int  fs_write(int fd, const void *buf, size_t len);
int  fs_read(int fd, void *buf, size_t len);
void fs_list(void);

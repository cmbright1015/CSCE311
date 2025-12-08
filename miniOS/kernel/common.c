#include "common.h"

size_t kstrlen(const char *s)
{
    size_t n = 0;
    while (s && s[n])
        n++;
    return n;
}

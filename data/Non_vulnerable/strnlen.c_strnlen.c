#include <string.h>
size_t strnlen (const char *str, size_t max)
{
    const char *end = memchr (str, 0, max);
    return end ? (size_t)(end - str) : max;
}

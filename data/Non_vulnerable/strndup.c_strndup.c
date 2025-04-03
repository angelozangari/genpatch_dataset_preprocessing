#include <stdlib.h>
char *strndup (const char *str, size_t max)
{
    size_t len = strnlen (str, max);
    char *res = malloc (len + 1);
    if (res)
    {
        memcpy (res, str, len);
        res[len] = '\0';
    }
    return res;
}

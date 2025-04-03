#include <stdlib.h>
char *strdup (const char *str)
{
    size_t len = strlen (str) + 1;
    char *res = malloc (len);
    if (res)
        memcpy (res, str, len);
    return res;
}

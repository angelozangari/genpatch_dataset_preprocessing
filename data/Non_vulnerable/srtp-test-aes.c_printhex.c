#include "srtp.c"
static void printhex (const void *buf, size_t len)
{
    for (size_t i = 0; i < len; i++)
        printf ("%02X", ((uint8_t *)buf)[i]);
    fputc ('\n', stdout);
}

#include <stdbool.h>
static void test (const char *in, const char *out)
{
    bool isutf8 = !strcmp (in, out);
    char *str = strdup (in);
    if (str == NULL)
        abort ();
    if (isutf8)
        printf ("\"%s\" should be accepted...\n", in);
    else
        printf ("\"%s\" should be rewritten as \"%s\"...\n", in, out);
    if ((IsUTF8 (in) != NULL) != isutf8)
    {
        printf (" ERROR: IsUTF8 (%s) failed\n", in);
        exit (1);
    }
    if ((EnsureUTF8 (str) != NULL) != isutf8)
    {
        printf (" ERROR: EnsureUTF8 (%s) failed\n", in);
        exit (2);
    }
    if (strcmp (str, out))
    {
        printf (" ERROR: got \"%s\"\n", str);
        exit (3);
    }
    if ((EnsureUTF8 (str) == NULL) || IsUTF8 (str) == NULL)
    {
        printf (" ERROR: EnsureUTF8 (%s) is not UTF-8\n", in);
        exit (4);
    }
    free (str);
}

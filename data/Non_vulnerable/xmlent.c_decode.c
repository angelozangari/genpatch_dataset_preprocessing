#include <vlc_strings.h>
static void decode (const char *in, const char *out)
{
    char buf[strlen (in) + 1];
    printf ("\"%s\" -> \"%s\" ?\n", in, out);
    strcpy (buf, in);
    resolve_xml_special_chars (buf);
    if (strcmp (buf, out))
    {
        printf (" ERROR: got \"%s\"\n", buf);
        exit (2);
    }
}

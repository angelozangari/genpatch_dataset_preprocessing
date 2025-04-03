}
static void encode (const char *in, const char *out)
{
    char *buf;
    printf ("\"%s\" -> \"%s\" ?\n", in, out);
    buf = convert_xml_special_chars (in);
    if (strcmp (buf, out))
    {
        printf (" ERROR: got \"%s\"\n", buf);
        exit (2);
    }
    free (buf);
}

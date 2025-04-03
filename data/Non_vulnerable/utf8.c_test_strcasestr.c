}
static void test_strcasestr (const char *h, const char *n, ssize_t offset)
{
    printf ("\"%s\" should %sbe found in \"%s\"...\n", n,
            (offset != -1) ? "" : "not ", h);
    const char *ret = vlc_strcasestr (h, n);
    if (offset == -1)
    {
        if (ret != NULL)
        {
            printf ("ERROR: got \"%s\"\n", ret);
            exit (10);
        }
    }
    else
    {
        if (ret == NULL)
        {
            printf ("ERROR: not found\n");
            exit (11);
        }
        if ((ret - h) != offset)
        {
            printf ("ERROR: got \"%s\" instead of \"%s\"\n",
                    ret, h + offset);
            exit (12);
        }
    }
}

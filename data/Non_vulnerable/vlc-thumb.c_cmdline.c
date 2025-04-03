/* extracts options from command line */
static void cmdline(int argc, const char **argv, const char **in,
                    char **out, char **out_with_ext, int *w)
{
    int idx = 1;
    size_t len;
    if (argc != 3 && argc != 5)
        usage(argv[0], argc != 2 || strcmp(argv[1], "-h"));
    *w = 0;
    if (argc == 5) {
        if (strcmp(argv[1], "-s"))
            usage(argv[0], 1);
        idx += 2; /* skip "-s width" */
        *w = atoi(argv[2]);
    }
    *in  = argv[idx++];
    *out = strdup(argv[idx++]);
    if (!*out)
        abort();
    len = strlen(*out);
    if (len >= 4 && !strcmp(*out + len - 4, ".png")) {
        *out_with_ext = *out;
        return;
    }
    /* We need to add .png extension to filename,
     * VLC relies on it to detect output format,
     * and nautilus doesn't give filenames ending in .png */
    *out_with_ext = malloc(len + sizeof ".png");
    if (!*out_with_ext)
        abort();
    strcpy(*out_with_ext, *out);
    strcat(*out_with_ext, ".png");
}

#define VLC_THUMBNAIL_POSITION (30./100.)
static void usage(const char *name, int ret)
{
    fprintf(stderr, "Usage: %s [-s width] <video> <output.png>\n", name);
    exit(ret);
}

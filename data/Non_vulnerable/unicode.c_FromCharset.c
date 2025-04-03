 */
char *FromCharset(const char *charset, const void *data, size_t data_size)
{
    vlc_iconv_t handle = vlc_iconv_open ("UTF-8", charset);
    if (handle == (vlc_iconv_t)(-1))
        return NULL;
    char *out = NULL;
    for(unsigned mul = 4; mul < 8; mul++ )
    {
        size_t in_size = data_size;
        const char *in = data;
        size_t out_max = mul * data_size;
        char *tmp = out = malloc (1 + out_max);
        if (!out)
            break;
        if (vlc_iconv (handle, &in, &in_size, &tmp, &out_max) != (size_t)(-1)) {
            *tmp = '\0';
            break;
        }
        free(out);
        out = NULL;
        if (errno != E2BIG)
            break;
    }
    vlc_iconv_close(handle);
    return out;
}

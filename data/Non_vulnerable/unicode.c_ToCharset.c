 */
void *ToCharset(const char *charset, const char *in, size_t *outsize)
{
    vlc_iconv_t hd = vlc_iconv_open (charset, "UTF-8");
    if (hd == (vlc_iconv_t)(-1))
        return NULL;
    const size_t inlen = strlen (in);
    void *res;
    for (unsigned mul = 4; mul < 16; mul++)
    {
        size_t outlen = mul * (inlen + 1);
        res = malloc (outlen);
        if (unlikely(res == NULL))
            break;
        const char *inp = in;
        char *outp = res;
        size_t inb = inlen;
        size_t outb = outlen - mul;
        if (vlc_iconv (hd, &inp, &inb, &outp, &outb) != (size_t)(-1))
        {
            *outsize = outlen - mul - outb;
            outb += mul;
            inb = 1; /* append nul terminator if possible */
            if (vlc_iconv (hd, &inp, &inb, &outp, &outb) != (size_t)(-1))
                break;
            if (errno == EILSEQ) /* cannot translate nul terminator!? */
                break;
        }
        free (res);
        res = NULL;
        if (errno != E2BIG) /* conversion failure */
            break;
    }
    vlc_iconv_close (hd);
    return res;
}

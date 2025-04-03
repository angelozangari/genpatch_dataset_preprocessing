static int
do_ctr_crypt (gcry_cipher_hd_t hd, const void *ctr, uint8_t *data, size_t len)
{
    const size_t ctrlen = 16;
    div_t d = div (len, ctrlen);
    if (gcry_cipher_setctr (hd, ctr, ctrlen)
     || gcry_cipher_encrypt (hd, data, d.quot * ctrlen, NULL, 0))
        return -1;
    if (d.rem)
    {
        /* Truncated last block */
        uint8_t dummy[ctrlen];
        data += d.quot * ctrlen;
        memcpy (dummy, data, d.rem);
        memset (dummy + d.rem, 0, ctrlen - d.rem);
        if (gcry_cipher_encrypt (hd, dummy, ctrlen, data, ctrlen))
            return -1;
        memcpy (data, dummy, d.rem);
    }
    return 0;
}

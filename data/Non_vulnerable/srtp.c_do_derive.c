static int
do_derive (gcry_cipher_hd_t prf, const void *salt,
           const uint8_t *r, size_t rlen, uint8_t label,
           void *out, size_t outlen)
{
    uint8_t iv[16];
    memcpy (iv, salt, 14);
    iv[14] = iv[15] = 0;
    assert (rlen < 14);
    iv[13 - rlen] ^= label;
    for (size_t i = 0; i < rlen; i++)
        iv[sizeof (iv) - rlen + i] ^= r[i];
    memset (out, 0, outlen);
    return do_ctr_crypt (prf, iv, out, outlen);
}

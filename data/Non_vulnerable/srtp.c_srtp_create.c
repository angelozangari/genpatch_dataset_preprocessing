srtp_session_t *
srtp_create (int encr, int auth, unsigned tag_len, int prf, unsigned flags)
{
    if ((flags & ~SRTP_FLAGS_MASK))
        return NULL;
    int cipher, md;
    switch (encr)
    {
        case SRTP_ENCR_NULL:
            cipher = GCRY_CIPHER_NONE;
            break;
        case SRTP_ENCR_AES_CM:
            cipher = GCRY_CIPHER_AES;
            break;
        default:
            return NULL;
    }
    switch (auth)
    {
        case SRTP_AUTH_NULL:
            md = GCRY_MD_NONE;
            break;
        case SRTP_AUTH_HMAC_SHA1:
            md = GCRY_MD_SHA1;
            break;
        default:
            return NULL;
    }
    if (tag_len > gcry_md_get_algo_dlen (md))
        return NULL;
    if (prf != SRTP_PRF_AES_CM)
        return NULL;
    srtp_session_t *s = malloc (sizeof (*s));
    if (s == NULL)
        return NULL;
    memset (s, 0, sizeof (*s));
    s->flags = flags;
    s->tag_len = tag_len;
    s->rtp_rcc = 1; /* Default RCC rate */
    if (rcc_mode (s))
    {
        if (tag_len < 4)
            goto error;
    }
    if (proto_create (&s->rtp, cipher, md) == 0)
    {
        if (proto_create (&s->rtcp, cipher, md) == 0)
            return s;
        proto_destroy (&s->rtp);
    }
error:
    free (s);
    return NULL;
}

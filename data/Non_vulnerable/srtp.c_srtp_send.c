int
srtp_send (srtp_session_t *s, uint8_t *buf, size_t *lenp, size_t bufsize)
{
    size_t len = *lenp;
    size_t tag_len;
    size_t roc_len = 0;
    /* Compute required buffer size */
    if (len < 12u)
        return EINVAL;
    if (!(s->flags & SRTP_UNAUTHENTICATED))
    {
        tag_len = s->tag_len;
        if (rcc_mode (s))
        {
            assert (tag_len >= 4);
            assert (s->rtp_rcc != 0);
            if ((rtp_seq (buf) % s->rtp_rcc) == 0)
            {
                roc_len = 4;
                if (rcc_mode (s) == 3)
                    tag_len = 0; /* RCC mode 3 -> no auth*/
                else
                    tag_len -= 4; /* RCC mode 1 or 2 -> auth*/
            }
            else
            {
                if (rcc_mode (s) & 1)
                    tag_len = 0; /* RCC mode 1 or 3 -> no auth */
            }
        }
        *lenp = len + roc_len + tag_len;
    }
    else
        tag_len = 0;
    if (bufsize < *lenp)
        return ENOSPC;
    /* Encrypt payload */
    int val = srtp_crypt (s, buf, len);
    if (val)
        return val;
    /* Authenticate payload */
    if (!(s->flags & SRTP_UNAUTHENTICATED))
    {
        uint32_t roc = srtp_compute_roc (s, rtp_seq (buf));
        const uint8_t *tag = rtp_digest (s->rtp.mac, buf, len, roc);
        if (roc_len)
        {
            memcpy (buf + len, &(uint32_t){ htonl (s->rtp_roc) }, 4);
            len += 4;
        }
        memcpy (buf + len, tag, tag_len);
#if 0
        printf ("Sent    : 0x");
        for (unsigned i = 0; i < tag_len; i++)
            printf ("%02x", tag[i]);
        puts ("");
#endif
    }
    return 0;
}

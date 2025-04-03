int
srtcp_send (srtp_session_t *s, uint8_t *buf, size_t *lenp, size_t bufsize)
{
    size_t len = *lenp;
    if (bufsize < (len + 4 + s->tag_len))
        return ENOSPC;
    uint32_t index = ++s->rtcp_index;
    if (index >> 31)
        s->rtcp_index = index = 0; /* 31-bit wrap */
    if ((s->flags & SRTCP_UNENCRYPTED) == 0)
        index |= 0x80000000; /* Set Encrypted bit */
    memcpy (buf + len, &(uint32_t){ htonl (index) }, 4);
    int val = srtcp_crypt (s, buf, len);
    if (val)
        return val;
    len += 4; /* Digests SRTCP index too */
    const uint8_t *tag = rtcp_digest (s->rtcp.mac, buf, len);
    memcpy (buf + len, tag, s->tag_len);
    *lenp = len + s->tag_len;
    return 0;
}

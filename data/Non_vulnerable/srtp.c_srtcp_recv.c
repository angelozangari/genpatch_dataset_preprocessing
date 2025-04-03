int
srtcp_recv (srtp_session_t *s, uint8_t *buf, size_t *lenp)
{
    size_t len = *lenp;
    if (len < (4u + s->tag_len))
        return EINVAL;
    len -= s->tag_len;
    const uint8_t *tag = rtcp_digest (s->rtcp.mac, buf, len);
    if (memcmp (buf + len, tag, s->tag_len))
         return EACCES;
    len -= 4; /* Remove SRTCP index before decryption */
    *lenp = len;
    return srtp_crypt (s, buf, len);
}

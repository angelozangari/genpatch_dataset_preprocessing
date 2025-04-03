 */
static int srtcp_crypt (srtp_session_t *s, uint8_t *buf, size_t len)
{
    assert (s != NULL);
    /* 8-bytes unencrypted header, and 4-bytes unencrypted footer */
    if ((len < 12) || ((buf[0] >> 6) != 2))
        return EINVAL;
    uint32_t index;
    memcpy (&index, buf + len, 4);
    index = ntohl (index);
    if (((index >> 31) != 0) != ((s->flags & SRTCP_UNENCRYPTED) == 0))
        return EINVAL; // E-bit mismatch
    index &= ~(1 << 31); // clear E-bit for counter
    /* Updates SRTCP index (safe here) */
    int32_t diff = index - s->rtcp_index;
    if (diff > 0)
    {
        /* Packet in the future, good */
        s->rtcp.window = s->rtcp.window << diff;
        s->rtcp.window |= UINT64_C(1);
        s->rtcp_index = index;
    }
    else
    {
        /* Packet in the past/present, bad */
        diff = -diff;
        if ((diff >= 64) || ((s->rtcp.window >> diff) & 1))
            return EACCES; // replay attack!
        s->rtp.window |= UINT64_C(1) << diff;
    }
    /* Crypts SRTCP */
    if (s->flags & SRTCP_UNENCRYPTED)
        return 0;
    uint32_t ssrc;
    memcpy (&ssrc, buf + 4, 4);
    if (rtcp_crypt (s->rtcp.cipher, ssrc, index, s->rtp.salt,
                    buf + 8, len - 8))
        return EINVAL;
    return 0;
}

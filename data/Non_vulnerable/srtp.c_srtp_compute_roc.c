static uint32_t
srtp_compute_roc (const srtp_session_t *s, uint16_t seq)
{
    uint32_t roc = s->rtp_roc;
    if (((seq - s->rtp_seq) & 0xffff) < 0x8000)
    {
        /* Sequence is ahead, good */
        if (seq < s->rtp_seq)
            roc++; /* Sequence number wrap */
    }
    else
    {
        /* Sequence is late, bad */
        if (seq > s->rtp_seq)
            roc--; /* Wrap back */
    }
    return roc;
}

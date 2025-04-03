};
static inline unsigned rcc_mode (const srtp_session_t *s)
{
    return (s->flags >> 4) & 3;
}

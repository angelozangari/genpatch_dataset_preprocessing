/** Returns RTP sequence (in host-byte order) */
static inline uint16_t rtp_seq (const uint8_t *buf)
{
    return (buf[2] << 8) | buf[3];
}

static int
rtp_crypt (gcry_cipher_hd_t hd, uint32_t ssrc, uint32_t roc, uint16_t seq,
           const uint32_t *salt, uint8_t *data, size_t len)
{
    /* Determines cryptographic counter (IV) */
    uint32_t counter[4];
    counter[0] = salt[0];
    counter[1] = salt[1] ^ ssrc;
    counter[2] = salt[2] ^ htonl (roc);
    counter[3] = salt[3] ^ htonl (seq << 16);
    /* Encryption */
    return do_ctr_crypt (hd, counter, data, len);
}

static int
rtcp_crypt (gcry_cipher_hd_t hd, uint32_t ssrc, uint32_t index,
            const uint32_t *salt, uint8_t *data, size_t len)
{
    return rtp_crypt (hd, ssrc, index >> 16, index & 0xffff, salt, data, len);
}

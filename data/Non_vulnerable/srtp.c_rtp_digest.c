static const uint8_t *
rtp_digest (gcry_md_hd_t md, const uint8_t *data, size_t len,
            uint32_t roc)
{
    gcry_md_reset (md);
    gcry_md_write (md, data, len);
    gcry_md_write (md, &(uint32_t){ htonl (roc) }, 4);
    return gcry_md_read (md, 0);
}

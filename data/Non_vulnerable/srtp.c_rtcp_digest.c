static const uint8_t *
rtcp_digest (gcry_md_hd_t md, const void *data, size_t len)
{
    gcry_md_reset (md);
    gcry_md_write (md, data, len);
    return gcry_md_read (md, 0);
}

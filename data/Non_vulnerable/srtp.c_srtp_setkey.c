int
srtp_setkey (srtp_session_t *s, const void *key, size_t keylen,
             const void *salt, size_t saltlen)
{
    /* SRTP/SRTCP cipher/salt/MAC keys derivation */
    gcry_cipher_hd_t prf;
    uint8_t r[6], keybuf[20];
    if (saltlen != 14)
        return EINVAL;
    if (gcry_cipher_open (&prf, GCRY_CIPHER_AES, GCRY_CIPHER_MODE_CTR, 0)
     || gcry_cipher_setkey (prf, key, keylen))
        return EINVAL;
    /* SRTP key derivation */
#if 0
    if (s->kdr != 0)
    {
        uint64_t index = (((uint64_t)s->rtp_roc) << 16) | s->rtp_seq;
        index /= s->kdr;
        for (int i = sizeof (r) - 1; i >= 0; i--)
        {
            r[i] = index & 0xff;
            index = index >> 8;
        }
    }
    else
#endif
        memset (r, 0, sizeof (r));
    if (do_derive (prf, salt, r, 6, SRTP_CRYPT, keybuf, 16)
     || gcry_cipher_setkey (s->rtp.cipher, keybuf, 16)
     || do_derive (prf, salt, r, 6, SRTP_AUTH, keybuf, 20)
     || gcry_md_setkey (s->rtp.mac, keybuf, 20)
     || do_derive (prf, salt, r, 6, SRTP_SALT, s->rtp.salt, 14))
        return -1;
    /* SRTCP key derivation */
    memcpy (r, &(uint32_t){ htonl (s->rtcp_index) }, 4);
    if (do_derive (prf, salt, r, 4, SRTCP_CRYPT, keybuf, 16)
     || gcry_cipher_setkey (s->rtcp.cipher, keybuf, 16)
     || do_derive (prf, salt, r, 4, SRTCP_AUTH, keybuf, 20)
     || gcry_md_setkey (s->rtcp.mac, keybuf, 20)
     || do_derive (prf, salt, r, 4, SRTCP_SALT, s->rtcp.salt, 14))
        return -1;
    (void)gcry_cipher_close (prf);
    return 0;
}

int
srtp_setkeystring (srtp_session_t *s, const char *key, const char *salt)
{
    uint8_t bkey[16]; /* TODO/NOTE: hard-coded for AES */
    uint8_t bsalt[14]; /* TODO/NOTE: hard-coded for the PRF-AES-CM */
    ssize_t bkeylen = hexstring (key, bkey, sizeof (bkey));
    ssize_t bsaltlen = hexstring (salt, bsalt, sizeof (bsalt));
    if ((bkeylen == -1) || (bsaltlen == -1))
        return EINVAL;
    return srtp_setkey (s, bkey, bkeylen, bsalt, bsaltlen) ? EINVAL : 0;
}

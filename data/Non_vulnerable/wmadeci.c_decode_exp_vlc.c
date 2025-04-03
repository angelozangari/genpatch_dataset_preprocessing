/* decode exponents coded with VLC codes */
static int decode_exp_vlc(WMADecodeContext *s, int ch)
{
    int last_exp, n, code;
    const uint16_t *ptr, *band_ptr;
    int32_t v, max_scale;
    int32_t *q,*q_end;
    /*accommodate the 60 negative indices */
    const int32_t *pow_10_to_yover16_ptr = &pow_10_to_yover16[61];
    band_ptr = s->exponent_bands[s->frame_len_bits - s->block_len_bits];
    ptr = band_ptr;
    q = s->exponents[ch];
    q_end = q + s->block_len;
    max_scale = 0;
    if (s->version == 1) /* wmav1 only */
    {
        last_exp = get_bits(&s->gb, 5) + 10;
        /* XXX: use a table */
        v = pow_10_to_yover16_ptr[last_exp];
        max_scale = v;
        n = *ptr++;
        do
        {
            *q++ = v;
        }
        while (--n);
    }
    else
        last_exp = 36;
    while (q < q_end)
    {
        code = get_vlc2(&s->gb, s->exp_vlc.table, EXPVLCBITS, EXPMAX);
        if (code < 0)
        {
            return -1;
        }
        /* NOTE: this offset is the same as MPEG4 AAC ! */
        last_exp += code - 60;
        /* XXX: use a table */
        v = pow_10_to_yover16_ptr[last_exp];
        if (v > max_scale)
        {
            max_scale = v;
        }
        n = *ptr++;
        do
        {
            *q++ = v;
        }
        while (--n);
    }
    s->max_exponent[ch] = max_scale;
    return 0;
}

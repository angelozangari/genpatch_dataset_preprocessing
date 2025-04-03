}
static void wma_lsp_to_curve_init(WMADecodeContext *s, int frame_len)
{
    int32_t wdel, a, b, temp, temp2;
    int i, m;
    wdel = fixdiv32(M_PI_F, itofix32(frame_len));
    temp = fixdiv32(itofix32(1),     itofix32(frame_len));
    for (i=0; i<frame_len; ++i)
    {
        /* TODO: can probably reuse the trig_init values here */
        fsincos((temp*i)<<15, &temp2);
        /* get 3 bits headroom + 1 bit from not doubleing the values */
        s->lsp_cos_table[i] = temp2>>3;
    }
    /* NOTE: these two tables are needed to avoid two operations in
       pow_m1_4 */
    b = itofix32(1);
    int ix = 0;
    /*double check this later*/
    for(i=(1 << LSP_POW_BITS) - 1;i>=0;i--)
    {
        m = (1 << LSP_POW_BITS) + i;
        a = pow_a_table[ix++]<<4;
        s->lsp_pow_m_table1[i] = 2 * a - b;
        s->lsp_pow_m_table2[i] = b - a;
        b = a;
    }
}

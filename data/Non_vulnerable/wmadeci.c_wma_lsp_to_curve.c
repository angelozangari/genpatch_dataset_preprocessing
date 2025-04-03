/* XXX: optimize it further with SSE/3Dnow */
static void wma_lsp_to_curve(WMADecodeContext *s,
                             int32_t *out,
                             int32_t *val_max_ptr,
                             int n,
                             int32_t *lsp)
{
    int i, j;
    int32_t p, q, w, v, val_max, temp, temp2;
    val_max = 0;
    for(i=0;i<n;++i)
    {
        /* shift by 2 now to reduce rounding error,
         * we can renormalize right before pow_m1_4
         */
        p = 0x8000<<5;
        q = 0x8000<<5;
        w = s->lsp_cos_table[i];
        for (j=1;j<NB_LSP_COEFS;j+=2)
        {
            /* w is 5.27 format, lsp is in 16.16, temp2 becomes 5.27 format */
            temp2 = ((w - (lsp[j - 1]<<11)));
            temp = q;
            /* q is 16.16 format, temp2 is 5.27, q becomes 16.16 */
            q = fixmul32b(q, temp2 )<<4;
            p = fixmul32b(p, (w - (lsp[j]<<11)))<<4;
        }
        /* 2 in 5.27 format is 0x10000000 */
        p = fixmul32(p, fixmul32b(p, (0x10000000 - w)))<<3;
        q = fixmul32(q, fixmul32b(q, (0x10000000 + w)))<<3;
        v = (p + q) >>9;  /* p/q end up as 16.16 */
        v = pow_m1_4(s, v);
        if (v > val_max)
            val_max = v;
        out[i] = v;
    }
    *val_max_ptr = val_max;
}

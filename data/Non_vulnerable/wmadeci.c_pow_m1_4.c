   bits of precision). */
static inline int32_t pow_m1_4(WMADecodeContext *s, int32_t x)
{
    union {
        float f;
        unsigned int v;
    } u, t;
    unsigned int e, m;
    int32_t a, b;
    u.f = fixtof64(x);
    e = u.v >> 23;
    m = (u.v >> (23 - LSP_POW_BITS)) & ((1 << LSP_POW_BITS) - 1);
    /* build interpolation scale: 1 <= t < 2. */
    t.v = ((u.v << LSP_POW_BITS) & ((1 << 23) - 1)) | (127 << 23);
    a = s->lsp_pow_m_table1[m];
    b = s->lsp_pow_m_table2[m];
    /* lsp_pow_e_table contains 32.32 format */
    /* TODO:  Since we're unlikely have value that cover the whole
     * IEEE754 range, we probably don't need to have all possible exponents */
    return (lsp_pow_e_table[e] * (a + fixmul32(b, ftofix32(t.f))) >>32);
}

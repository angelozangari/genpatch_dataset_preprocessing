/* decode exponents coded with LSP coefficients (same idea as Vorbis) */
static void decode_exp_lsp(WMADecodeContext *s, int ch)
{
    int32_t lsp_coefs[NB_LSP_COEFS];
    int val, i;
    for (i = 0; i < NB_LSP_COEFS; ++i)
    {
        if (i == 0 || i >= 8)
            val = get_bits(&s->gb, 3);
        else
            val = get_bits(&s->gb, 4);
        lsp_coefs[i] = lsp_codebook[i][val];
    }
    wma_lsp_to_curve(s,
                     s->exponents[ch],
                     &s->max_exponent[ch],
                     s->block_len,
                     lsp_coefs);
}

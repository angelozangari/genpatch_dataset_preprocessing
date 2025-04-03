}
static block_t *ExtractLeft( filter_t *p_filter, block_t *p_buf )
{
    float *p_dest = (float *)p_buf->p_buffer;
    const float *p_src = p_dest;
    for( unsigned i = 0; i < p_buf->i_nb_samples; i++ )
    {
        *(p_dest++) = *p_src;
        p_src += 2;
    }
    (void) p_filter;
    return p_buf;
}

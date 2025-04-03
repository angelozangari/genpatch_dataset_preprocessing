}
static block_t *ExtractRight( filter_t *p_filter, block_t *p_buf )
{
    float *p_dest = (float *)p_buf->p_buffer;
    const float *p_src = p_dest;
    for( unsigned i = 0; i < p_buf->i_nb_samples; i++ )
    {
        p_src++;
        *(p_dest++) = *(p_src++);
    }
    (void) p_filter;
    return p_buf;
}

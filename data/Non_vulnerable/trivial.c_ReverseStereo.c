}
static block_t *ReverseStereo( filter_t *p_filter, block_t *p_buf )
{
    float *p = (float *)p_buf->p_buffer;
    /* Reverse-stereo mode */
    for( unsigned i = 0; i < p_buf->i_nb_samples; i++ )
    {
        float f = p[0];
        p[0] = p[1];
        p[1] = f;
        p += 2;
    }
    (void) p_filter;
    return p_buf;
}

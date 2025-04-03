 */
static block_t *Downmix( filter_t *p_filter, block_t *p_buf )
{
    unsigned i_input_nb = aout_FormatNbChannels( &p_filter->fmt_in.audio );
    unsigned i_output_nb = aout_FormatNbChannels( &p_filter->fmt_out.audio );
    assert( i_input_nb >= i_output_nb );
    float *p_dest = (float *)p_buf->p_buffer;
    const float *p_src = p_dest;
    for( size_t i = 0; i < p_buf->i_nb_samples; i++ )
    {
        for( unsigned j = 0; j < i_output_nb; j++ )
            p_dest[j] = p_src[j];
        p_src += i_input_nb;
        p_dest += i_output_nb;
    }
    return p_buf;
}

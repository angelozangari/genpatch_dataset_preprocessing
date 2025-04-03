 *****************************************************************************/
static block_t *DoWork( filter_t * p_filter, block_t * p_in_buf )
{
    /* Check if we really need to run the resampler */
    if( p_filter->fmt_out.audio.i_rate == p_filter->fmt_in.audio.i_rate )
        return p_in_buf;
    block_t *p_out_buf = p_in_buf;
    unsigned int i_out_nb = p_in_buf->i_nb_samples
        * p_filter->fmt_out.audio.i_rate / p_filter->fmt_in.audio.i_rate;
    const unsigned framesize = (p_filter->fmt_in.audio.i_bitspersample / 8)
        * aout_FormatNbChannels( &p_filter->fmt_in.audio );
    if( p_filter->fmt_out.audio.i_rate > p_filter->fmt_in.audio.i_rate )
    {
        p_out_buf = block_Alloc( i_out_nb * framesize );
        if( !p_out_buf )
            goto out;
    }
    unsigned char *p_out = p_out_buf->p_buffer;
    unsigned char *p_in = p_in_buf->p_buffer;
    unsigned int i_remainder = 0;
    p_out_buf->i_nb_samples = i_out_nb;
    p_out_buf->i_buffer = i_out_nb * framesize;
    p_out_buf->i_pts = p_in_buf->i_pts;
    p_out_buf->i_length = p_out_buf->i_nb_samples *
        1000000 / p_filter->fmt_out.audio.i_rate;
    while( i_out_nb )
    {
        if( p_out != p_in )
            memcpy( p_out, p_in, framesize );
        p_out += framesize;
        i_out_nb--;
        i_remainder += p_filter->fmt_in.audio.i_rate;
        while( i_remainder >= p_filter->fmt_out.audio.i_rate )
        {
            p_in += framesize;
            i_remainder -= p_filter->fmt_out.audio.i_rate;
        }
    }
    if( p_in_buf != p_out_buf )
out:
        block_Release( p_in_buf );
    return p_out_buf;
}

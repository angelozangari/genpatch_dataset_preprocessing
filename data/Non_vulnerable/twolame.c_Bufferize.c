 ****************************************************************************/
static void Bufferize( encoder_t *p_enc, int16_t *p_in, int i_nb_samples )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    const unsigned i_offset = p_sys->i_nb_samples * p_enc->fmt_in.audio.i_channels;
    const unsigned i_len = ARRAY_SIZE(p_sys->p_buffer);
    if( i_offset >= i_len )
    {
        msg_Err( p_enc, "buffer full" );
        return;
    }
    unsigned i_copy = i_nb_samples * p_enc->fmt_in.audio.i_channels;
    if( i_copy + i_offset > i_len)
    {
        msg_Err( p_enc, "dropping samples" );
        i_copy = i_len - i_offset;
    }
    memcpy( p_sys->p_buffer + i_offset, p_in, i_copy * sizeof(int16_t) );
}

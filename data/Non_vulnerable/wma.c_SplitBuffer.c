 *****************************************************************************/
static block_t *SplitBuffer( decoder_t *p_dec )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    unsigned int i_samples = __MIN( p_sys->i_samples, 2048 );
    block_t *p_buffer;
    if( i_samples == 0 ) return NULL;
    if( !( p_buffer = decoder_NewAudioBuffer( p_dec, i_samples ) ) )
        return NULL;
    p_buffer->i_pts = date_Get( &p_sys->end_date );
    p_buffer->i_length = date_Increment( &p_sys->end_date, i_samples )
                         - p_buffer->i_pts;
    memcpy( p_buffer->p_buffer, p_sys->p_samples, p_buffer->i_buffer );
    p_sys->p_samples += p_buffer->i_buffer;
    p_sys->i_samples -= i_samples;
    return p_buffer;
}

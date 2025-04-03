 ****************************************************************************/
static block_t *Encode( encoder_t *p_enc, block_t *p_aout_buf )
{
    encoder_sys_t *p_sys = p_enc->p_sys;
    ogg_packet oggpacket;
    block_t *p_block, *p_chain = NULL;
    float **buffer;
    /* FIXME: flush buffers in here */
    if( unlikely( !p_aout_buf ) ) return NULL;
    mtime_t i_pts = p_aout_buf->i_pts -
                (mtime_t)1000000 * (mtime_t)p_sys->i_samples_delay /
                (mtime_t)p_enc->fmt_in.audio.i_rate;
    p_sys->i_samples_delay += p_aout_buf->i_nb_samples;
    buffer = vorbis_analysis_buffer( &p_sys->vd, p_aout_buf->i_nb_samples );
    /* convert samples to float and uninterleave */
    for( unsigned int i = 0; i < p_sys->i_channels; i++ )
    {
        for( unsigned int j = 0 ; j < p_aout_buf->i_nb_samples ; j++ )
        {
            buffer[i][j]= ((float *)p_aout_buf->p_buffer)
                                    [j * p_sys->i_channels + p_sys->pi_chan_table[i]];
        }
    }
    vorbis_analysis_wrote( &p_sys->vd, p_aout_buf->i_nb_samples );
    while( vorbis_analysis_blockout( &p_sys->vd, &p_sys->vb ) == 1 )
    {
        int i_samples;
        vorbis_analysis( &p_sys->vb, NULL );
        vorbis_bitrate_addblock( &p_sys->vb );
        while( vorbis_bitrate_flushpacket( &p_sys->vd, &oggpacket ) )
        {
            int i_block_size;
            p_block = block_Alloc( oggpacket.bytes );
            memcpy( p_block->p_buffer, oggpacket.packet, oggpacket.bytes );
            i_block_size = vorbis_packet_blocksize( &p_sys->vi, &oggpacket );
            if( i_block_size < 0 ) i_block_size = 0;
            i_samples = ( p_sys->i_last_block_size + i_block_size ) >> 2;
            p_sys->i_last_block_size = i_block_size;
            p_block->i_length = (mtime_t)1000000 *
                (mtime_t)i_samples / (mtime_t)p_enc->fmt_in.audio.i_rate;
            p_block->i_dts = p_block->i_pts = i_pts;
            p_sys->i_samples_delay -= i_samples;
            /* Update pts */
            i_pts += p_block->i_length;
            block_ChainAppend( &p_chain, p_block );
        }
    }
    return p_chain;
}

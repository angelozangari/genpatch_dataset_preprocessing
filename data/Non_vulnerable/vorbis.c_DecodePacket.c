 *****************************************************************************/
static block_t *DecodePacket( decoder_t *p_dec, ogg_packet *p_oggpacket )
{
    decoder_sys_t *p_sys = p_dec->p_sys;
    int           i_samples;
    INTERLEAVE_TYPE **pp_pcm;
    if( p_oggpacket->bytes &&
        vorbis_synthesis( &p_sys->vb, p_oggpacket ) == 0 )
        vorbis_synthesis_blockin( &p_sys->vd, &p_sys->vb );
    /* **pp_pcm is a multichannel float vector. In stereo, for
     * example, pp_pcm[0] is left, and pp_pcm[1] is right. i_samples is
     * the size of each channel. Convert the float values
     * (-1.<=range<=1.) to whatever PCM format and write it out */
    if( ( i_samples = vorbis_synthesis_pcmout( &p_sys->vd, &pp_pcm ) ) > 0 )
    {
        block_t *p_aout_buffer;
        p_aout_buffer =
            decoder_NewAudioBuffer( p_dec, i_samples );
        if( p_aout_buffer == NULL ) return NULL;
        /* Interleave the samples */
        Interleave( (INTERLEAVE_TYPE*)p_aout_buffer->p_buffer,
                    (const INTERLEAVE_TYPE**)pp_pcm, p_sys->vi.channels, i_samples,
                    p_sys->pi_chan_table);
        /* Tell libvorbis how many samples we actually consumed */
        vorbis_synthesis_read( &p_sys->vd, i_samples );
        /* Date management */
        p_aout_buffer->i_pts = date_Get( &p_sys->end_date );
        p_aout_buffer->i_length = date_Increment( &p_sys->end_date,
                                           i_samples ) - p_aout_buffer->i_pts;
        return p_aout_buffer;
    }
    else
    {
        return NULL;
    }
}

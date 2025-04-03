 *****************************************************************************/
static void Play( audio_output_t *p_aout, block_t *block )
{
    struct lkwavehdr * p_waveheader =
        (struct lkwavehdr *) malloc(sizeof(struct lkwavehdr));
    if(!p_waveheader)
    {
        msg_Err(p_aout, "Couldn't alloc WAVEHDR");
        if( block )
            block_Release( block );
        return;
    }
    p_waveheader->p_next = NULL;
    if( block && p_aout->sys->chans_to_reorder )
    {
        aout_ChannelReorder( block->p_buffer, block->i_buffer,
                             p_aout->sys->waveformat.Format.nChannels,
                             p_aout->sys->chan_table, p_aout->sys->format );
    }
    while( PlayWaveOut( p_aout, p_aout->sys->h_waveout, p_waveheader, block,
                        p_aout->sys->b_spdif ) != VLC_SUCCESS )
    {
        msg_Warn( p_aout, "Couln't write frame... sleeping");
        msleep( block->i_length );
    }
    WaveOutClean( p_aout->sys );
    WaveoutPollVolume( p_aout );
    vlc_mutex_lock( &p_aout->sys->lock );
    p_aout->sys->i_frames++;
    p_aout->sys->i_played_length += block->i_length;
    vlc_mutex_unlock( &p_aout->sys->lock );
}

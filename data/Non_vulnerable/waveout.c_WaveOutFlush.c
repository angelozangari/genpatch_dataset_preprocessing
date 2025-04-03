}
static void WaveOutFlush( audio_output_t *p_aout, bool wait)
{
    MMRESULT res;
    if( !wait )
    {
        res  = waveOutReset( p_aout->sys->h_waveout );
        p_aout->sys->i_played_length = 0;
        if( res != MMSYSERR_NOERROR )
            msg_Err( p_aout, "waveOutReset failed");
    }
    else
    {
        vlc_mutex_lock( &p_aout->sys->lock );
        while( p_aout->sys->i_frames )
        {
            vlc_cond_wait( &p_aout->sys->cond, &p_aout->sys-> lock );
        }
        vlc_mutex_unlock( &p_aout->sys->lock );
    }
}

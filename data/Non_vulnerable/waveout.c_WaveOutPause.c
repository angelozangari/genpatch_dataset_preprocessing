}
static void WaveOutPause( audio_output_t * p_aout, bool pause, mtime_t date)
{
    MMRESULT res;
    (void) date;
    if(pause)
    {
        vlc_timer_schedule( p_aout->sys->volume_poll_timer, false, 1, 200000 );
        res = waveOutPause( p_aout->sys->h_waveout );
        if( res != MMSYSERR_NOERROR )
        {
            msg_Err( p_aout, "waveOutPause failed (0x%x)", res);
            return;
        }
    }
    else
    {
        vlc_timer_schedule( p_aout->sys->volume_poll_timer, false, 0, 0 );
        res = waveOutRestart( p_aout->sys->h_waveout );
        if( res != MMSYSERR_NOERROR )
        {
            msg_Err( p_aout, "waveOutRestart failed (0x%x)", res);
            return;
        }
    }
}

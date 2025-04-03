}
static int WaveOutTimeGet(audio_output_t * p_aout, mtime_t *delay)
{
    MMTIME mmtime;
    mmtime.wType = TIME_SAMPLES;
    if( !p_aout->sys->i_frames )
        return -1;
    if( waveOutGetPosition( p_aout->sys->h_waveout, &mmtime, sizeof(MMTIME) )
            != MMSYSERR_NOERROR )
    {
        msg_Err( p_aout, "waveOutGetPosition failed");
        return -1;
    }
    mtime_t i_pos = (mtime_t) mmtime.u.sample * CLOCK_FREQ / p_aout->sys->i_rate;
    *delay = p_aout->sys->i_played_length - i_pos;
    return 0;
}

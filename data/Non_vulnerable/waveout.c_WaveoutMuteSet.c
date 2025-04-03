}
static int WaveoutMuteSet( audio_output_t * p_aout, bool mute )
{
    aout_sys_t *sys = p_aout->sys;
    if( sys->b_soft )
    {
        float gain = sys->f_volume * sys->f_volume * sys->f_volume;
        if ( aout_GainRequest( p_aout, mute ? 0.f : gain ) )
            return -1;
    }
    else
    {
        const HWAVEOUT hwo = sys->h_waveout;
        uint32_t vol = mute ? 0 : lroundf( sys->f_volume * 0x7fff.fp0 );
        if( vol > 0xffff )
            vol = 0xffff;
        MMRESULT r = waveOutSetVolume( hwo, vol | ( vol << 16 ) );
        if( r != MMSYSERR_NOERROR )
        {
            msg_Err( p_aout, "waveOutSetVolume failed (%u)", r );
            return -1;
        }
    }
    vlc_mutex_lock(&p_aout->sys->lock);
    sys->b_mute = mute;
    aout_MuteReport( p_aout, mute );
    vlc_mutex_unlock(&p_aout->sys->lock);
    return 0;
}

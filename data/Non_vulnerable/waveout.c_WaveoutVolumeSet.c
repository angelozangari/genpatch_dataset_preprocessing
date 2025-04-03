}
static int WaveoutVolumeSet( audio_output_t *p_aout, float volume )
{
    aout_sys_t *sys = p_aout->sys;
    if( sys->b_soft )
    {
        float gain = volume * volume * volume;
        if ( !sys->b_mute && aout_GainRequest( p_aout, gain ) )
            return -1;
    }
    else
    {
        const HWAVEOUT hwo = sys->h_waveout;
        uint32_t vol = lroundf( volume * 0x7fff.fp0 );
        if( !sys->b_mute )
        {
            if( vol > 0xffff )
            {
                vol = 0xffff;
                volume = 2.0f;
            }
            MMRESULT r = waveOutSetVolume( hwo, vol | ( vol << 16 ) );
            if( r != MMSYSERR_NOERROR )
            {
                msg_Err( p_aout, "waveOutSetVolume failed (%u)", r );
                return -1;
            }
        }
    }
    vlc_mutex_lock(&p_aout->sys->lock);
    sys->f_volume = volume;
    if( var_InheritBool( p_aout, "volume-save" ) )
        config_PutFloat( p_aout, "waveout-volume", volume );
    aout_VolumeReport( p_aout, volume );
    vlc_mutex_unlock(&p_aout->sys->lock);
    return 0;
}

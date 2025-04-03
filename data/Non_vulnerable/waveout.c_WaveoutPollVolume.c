}
static void WaveoutPollVolume( void * aout )
{
    audio_output_t * p_aout = (audio_output_t *) aout;
    uint32_t vol;
    MMRESULT r = waveOutGetVolume( p_aout->sys->h_waveout, (LPDWORD) &vol );
    if( r != MMSYSERR_NOERROR )
    {
        msg_Err( p_aout, "waveOutGetVolume failed (%u)", r );
        return;
    }
    float volume = (float) ( vol & UINT32_C( 0xffff ) );
    volume /= 0x7fff.fp0;
    vlc_mutex_lock(&p_aout->sys->lock);
    if( !p_aout->sys->b_mute && volume != p_aout->sys->f_volume )
    {
        p_aout->sys->f_volume = volume;
        if( var_InheritBool( p_aout, "volume-save" ) )
            config_PutFloat( p_aout, "waveout-volume", volume );
        aout_VolumeReport( p_aout, volume );
    }
    vlc_mutex_unlock(&p_aout->sys->lock);
    return;
}

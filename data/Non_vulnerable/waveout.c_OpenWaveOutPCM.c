 ****************************************************************************/
static int OpenWaveOutPCM( audio_output_t *p_aout, uint32_t i_device_id,
                           vlc_fourcc_t *i_format,
                           int i_channels, int i_nb_channels, int i_rate,
                           bool b_probe )
{
    bool b_use_float32 = var_CreateGetBool( p_aout, "waveout-float32");
    if( !b_use_float32 || OpenWaveOut( p_aout, i_device_id, VLC_CODEC_FL32,
                                   i_channels, i_nb_channels, i_rate, b_probe )
        != VLC_SUCCESS )
    {
        if ( OpenWaveOut( p_aout, i_device_id, VLC_CODEC_S16N,
                          i_channels, i_nb_channels, i_rate, b_probe )
             != VLC_SUCCESS )
        {
            return VLC_EGENERIC;
        }
        else
        {
            *i_format = VLC_CODEC_S16N;
            return VLC_SUCCESS;
        }
    }
    else
    {
        *i_format = VLC_CODEC_FL32;
        return VLC_SUCCESS;
    }
}

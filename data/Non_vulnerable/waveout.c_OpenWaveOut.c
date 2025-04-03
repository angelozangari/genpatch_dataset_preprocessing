 ****************************************************************************/
static int OpenWaveOut( audio_output_t *p_aout, uint32_t i_device_id, int i_format,
                        int i_channels, int i_nb_channels, int i_rate,
                        bool b_probe )
{
    MMRESULT result;
    /* Set sound format */
#define waveformat p_aout->sys->waveformat
    waveformat.dwChannelMask = 0;
    for( unsigned i = 0; pi_vlc_chan_order_wg4[i]; i++ )
        if( i_channels & pi_vlc_chan_order_wg4[i] )
            waveformat.dwChannelMask |= pi_channels_in[i];
    switch( i_format )
    {
    case VLC_CODEC_SPDIFL:
        i_nb_channels = 2;
        /* To prevent channel re-ordering */
        waveformat.dwChannelMask = SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT;
        waveformat.Format.wBitsPerSample = 16;
        waveformat.Samples.wValidBitsPerSample =
            waveformat.Format.wBitsPerSample;
        waveformat.Format.wFormatTag = WAVE_FORMAT_DOLBY_AC3_SPDIF;
        waveformat.SubFormat = __KSDATAFORMAT_SUBTYPE_DOLBY_AC3_SPDIF;
        break;
    case VLC_CODEC_FL32:
        waveformat.Format.wBitsPerSample = sizeof(float) * 8;
        waveformat.Samples.wValidBitsPerSample =
            waveformat.Format.wBitsPerSample;
        waveformat.Format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
        waveformat.SubFormat = __KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
        break;
    case VLC_CODEC_S16N:
        waveformat.Format.wBitsPerSample = 16;
        waveformat.Samples.wValidBitsPerSample =
            waveformat.Format.wBitsPerSample;
        waveformat.Format.wFormatTag = WAVE_FORMAT_PCM;
        waveformat.SubFormat = __KSDATAFORMAT_SUBTYPE_PCM;
        break;
    }
    waveformat.Format.nChannels = i_nb_channels;
    waveformat.Format.nSamplesPerSec = i_rate;
    waveformat.Format.nBlockAlign =
        waveformat.Format.wBitsPerSample / 8 * i_nb_channels;
    waveformat.Format.nAvgBytesPerSec =
        waveformat.Format.nSamplesPerSec * waveformat.Format.nBlockAlign;
    /* Only use the new WAVE_FORMAT_EXTENSIBLE format for multichannel audio */
    if( i_nb_channels <= 2 )
    {
        waveformat.Format.cbSize = 0;
    }
    else
    {
        waveformat.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
        waveformat.Format.cbSize =
            sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX);
    }
    if(!b_probe) {
        msg_Dbg( p_aout, "OpenWaveDevice-ID: %u", i_device_id);
        msg_Dbg( p_aout,"waveformat.Format.cbSize          = %d",
                 waveformat.Format.cbSize);
        msg_Dbg( p_aout,"waveformat.Format.wFormatTag      = %u",
                 waveformat.Format.wFormatTag);
        msg_Dbg( p_aout,"waveformat.Format.nChannels       = %u",
                 waveformat.Format.nChannels);
        msg_Dbg( p_aout,"waveformat.Format.nSamplesPerSec  = %d",
                 (int)waveformat.Format.nSamplesPerSec);
        msg_Dbg( p_aout,"waveformat.Format.nAvgBytesPerSec = %u",
                 (int)waveformat.Format.nAvgBytesPerSec);
        msg_Dbg( p_aout,"waveformat.Format.nBlockAlign     = %d",
                 waveformat.Format.nBlockAlign);
        msg_Dbg( p_aout,"waveformat.Format.wBitsPerSample  = %d",
                 waveformat.Format.wBitsPerSample);
        msg_Dbg( p_aout,"waveformat.Samples.wValidBitsPerSample = %d",
                 waveformat.Samples.wValidBitsPerSample);
        msg_Dbg( p_aout,"waveformat.Samples.wSamplesPerBlock = %d",
                 waveformat.Samples.wSamplesPerBlock);
        msg_Dbg( p_aout,"waveformat.dwChannelMask          = %u",
                 waveformat.dwChannelMask);
    }
    /* Open the device */
    result = waveOutOpen( &p_aout->sys->h_waveout, i_device_id,
                          (WAVEFORMATEX *)&waveformat,
                          (DWORD_PTR)WaveOutCallback, (DWORD_PTR)p_aout,
                          CALLBACK_FUNCTION | (b_probe?WAVE_FORMAT_QUERY:0) );
    if( result == WAVERR_BADFORMAT )
    {
        msg_Warn( p_aout, "waveOutOpen failed WAVERR_BADFORMAT" );
        return VLC_EGENERIC;
    }
    if( result == MMSYSERR_ALLOCATED )
    {
        msg_Warn( p_aout, "waveOutOpen failed WAVERR_ALLOCATED" );
        return VLC_EGENERIC;
    }
    if( result != MMSYSERR_NOERROR )
    {
        msg_Warn( p_aout, "waveOutOpen failed" );
        return VLC_EGENERIC;
    }
    p_aout->sys->chans_to_reorder =
        aout_CheckChannelReorder( pi_channels_in, pi_channels_out,
                                  waveformat.dwChannelMask,
                                  p_aout->sys->chan_table );
    if( p_aout->sys->chans_to_reorder )
        msg_Dbg( p_aout, "channel reordering needed" );
    p_aout->sys->format = i_format;
    return VLC_SUCCESS;
#undef waveformat
}

};
static void vlc_ToWave(WAVEFORMATEXTENSIBLE *restrict wf,
                       audio_sample_format_t *restrict audio)
{
    switch (audio->i_format)
    {
        case VLC_CODEC_FL64:
            audio->i_format = VLC_CODEC_FL32;
        case VLC_CODEC_FL32:
            wf->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
            break;
        case VLC_CODEC_U8:
            audio->i_format = VLC_CODEC_S16N;
        case VLC_CODEC_S16N:
            wf->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
            break;
        default:
            audio->i_format = VLC_CODEC_FL32;
            wf->SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
            break;
    }
    aout_FormatPrepare (audio);
    wf->Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
    wf->Format.nChannels = audio->i_channels;
    wf->Format.nSamplesPerSec = audio->i_rate;
    wf->Format.nAvgBytesPerSec = audio->i_bytes_per_frame * audio->i_rate;
    wf->Format.nBlockAlign = audio->i_bytes_per_frame;
    wf->Format.wBitsPerSample = audio->i_bitspersample;
    wf->Format.cbSize = sizeof (*wf) - sizeof (wf->Format);
    wf->Samples.wValidBitsPerSample = audio->i_bitspersample;
    wf->dwChannelMask = 0;
    for (unsigned i = 0; pi_vlc_chan_order_wg4[i]; i++)
        if (audio->i_physical_channels & pi_vlc_chan_order_wg4[i])
            wf->dwChannelMask |= chans_in[i];
}

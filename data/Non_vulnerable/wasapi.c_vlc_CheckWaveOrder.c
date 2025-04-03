}
static unsigned vlc_CheckWaveOrder (const WAVEFORMATEX *restrict wf,
                                    uint8_t *restrict table)
{
    uint32_t mask = 0;
    if (wf->wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        const WAVEFORMATEXTENSIBLE *wfe = (void *)wf;
        mask = wfe->dwChannelMask;
    }
    return aout_CheckChannelReorder(chans_in, chans_out, mask, table);
}

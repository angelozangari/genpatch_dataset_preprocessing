 */
static int Create( vlc_object_t *p_this )
{
    filter_t *p_filter = (filter_t *)p_this;
    const audio_format_t *infmt = &p_filter->fmt_in.audio;
    const audio_format_t *outfmt = &p_filter->fmt_out.audio;
    if( infmt->i_format != outfmt->i_format
     || infmt->i_rate != outfmt->i_rate
     || infmt->i_format != VLC_CODEC_FL32 )
        return VLC_EGENERIC;
    if( infmt->i_physical_channels == outfmt->i_physical_channels
     && infmt->i_original_channels == outfmt->i_original_channels )
        return VLC_EGENERIC;
    if( outfmt->i_physical_channels == AOUT_CHANS_STEREO )
    {
        bool swap = (outfmt->i_original_channels & AOUT_CHAN_REVERSESTEREO)
                  != (infmt->i_original_channels & AOUT_CHAN_REVERSESTEREO);
        if( (outfmt->i_original_channels & AOUT_CHAN_PHYSMASK)
                                                            == AOUT_CHAN_LEFT )
        {
            p_filter->pf_audio_filter = swap ? CopyRight : CopyLeft;
            return VLC_SUCCESS;
        }
        if( (outfmt->i_original_channels & AOUT_CHAN_PHYSMASK)
                                                           == AOUT_CHAN_RIGHT )
        {
            p_filter->pf_audio_filter = swap ? CopyLeft : CopyRight;
            return VLC_SUCCESS;
        }
        if( swap )
        {
            p_filter->pf_audio_filter = ReverseStereo;
            return VLC_SUCCESS;
        }
    }
    if ( aout_FormatNbChannels( outfmt ) == 1 )
    {
        bool mono = !!(infmt->i_original_channels & AOUT_CHAN_DUALMONO);
        if( mono && (infmt->i_original_channels & AOUT_CHAN_LEFT) )
        {
            p_filter->pf_audio_filter = ExtractLeft;
            return VLC_SUCCESS;
        }
        if( mono && (infmt->i_original_channels & AOUT_CHAN_RIGHT) )
        {
            p_filter->pf_audio_filter = ExtractRight;
            return VLC_SUCCESS;
        }
    }
    if( aout_FormatNbChannels( outfmt ) > aout_FormatNbChannels( infmt ) )
        p_filter->pf_audio_filter = Upmix;
    else
        p_filter->pf_audio_filter = Downmix;
    return VLC_SUCCESS;
}

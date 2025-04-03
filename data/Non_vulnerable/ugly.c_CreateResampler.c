}
static int CreateResampler( vlc_object_t *p_this )
{
    filter_t * p_filter = (filter_t *)p_this;
    if( p_filter->fmt_in.audio.i_format != p_filter->fmt_out.audio.i_format
     || p_filter->fmt_in.audio.i_physical_channels
                                 != p_filter->fmt_out.audio.i_physical_channels
     || p_filter->fmt_in.audio.i_original_channels
                                 != p_filter->fmt_out.audio.i_original_channels
     || !AOUT_FMT_LINEAR( &p_filter->fmt_in.audio ) )
        return VLC_EGENERIC;
    p_filter->pf_audio_filter = DoWork;
    return VLC_SUCCESS;
}

}
static int OpenResampler (vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    /* Only float->float */
    if (filter->fmt_in.audio.i_format != VLC_CODEC_FL32
     || filter->fmt_out.audio.i_format != VLC_CODEC_FL32
    /* No channels remapping */
     || filter->fmt_in.audio.i_physical_channels
                                  != filter->fmt_out.audio.i_physical_channels
     || filter->fmt_in.audio.i_original_channels
                                  != filter->fmt_out.audio.i_original_channels)
        return VLC_EGENERIC;
    int type = var_InheritInteger (obj, "src-converter-type");
    int channels = aout_FormatNbChannels (&filter->fmt_in.audio);
    int err;
    SRC_STATE *s = src_new (type, channels, &err);
    if (s == NULL)
    {
        msg_Err (obj, "cannot initialize resampler: %s", src_strerror (err));
        return VLC_EGENERIC;
    }
    filter->p_sys = (filter_sys_t *)s;
    filter->pf_audio_filter = Resample;
    return VLC_SUCCESS;
}

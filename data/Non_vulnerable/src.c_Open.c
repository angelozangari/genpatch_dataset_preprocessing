static block_t *Resample (filter_t *, block_t *);
static int Open (vlc_object_t *obj)
{
    filter_t *filter = (filter_t *)obj;
    /* Will change rate */
    if (filter->fmt_in.audio.i_rate == filter->fmt_out.audio.i_rate)
        return VLC_EGENERIC;
    return OpenResampler (obj);
}

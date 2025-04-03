 *****************************************************************************/
static int Create( vlc_object_t *p_this )
{
    filter_t * p_filter = (filter_t *)p_this;
    if( p_filter->fmt_in.audio.i_rate == p_filter->fmt_out.audio.i_rate )
        return VLC_EGENERIC;
    return CreateResampler( p_this );
}

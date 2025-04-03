 *****************************************************************************/
static int PacketizerOpen( vlc_object_t *p_this )
{
    if( DecoderOpen( p_this ) != VLC_SUCCESS ) return VLC_EGENERIC;
    return VLC_SUCCESS;
}

 *****************************************************************************/
int Import_ZPL( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    if(! ( demux_IsPathExtension( p_demux, ".zpl" ) || demux_IsForced( p_demux,  "zpl" )))
        return VLC_EGENERIC;
    STANDARD_DEMUX_INIT_MSG( "found valid ZPL playlist" );
    p_demux->p_sys->psz_prefix = FindPrefix( p_demux );
    return VLC_SUCCESS;
}

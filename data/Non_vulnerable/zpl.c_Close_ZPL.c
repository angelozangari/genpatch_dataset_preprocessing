 *****************************************************************************/
void Close_ZPL( vlc_object_t *p_this )
{
    demux_t *p_demux = (demux_t *)p_this;
    free( p_demux->p_sys->psz_prefix );
    free( p_demux->p_sys );
}

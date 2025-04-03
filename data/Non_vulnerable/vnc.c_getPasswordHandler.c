/* Auth */
static char *getPasswordHandler( rfbClient *p_client )
{
    demux_t *p_demux = (demux_t *) rfbClientGetClientData( p_client, DemuxThread );
    /* freed by libvnc */
    return var_InheritString( p_demux, CFG_PREFIX "password" );
}

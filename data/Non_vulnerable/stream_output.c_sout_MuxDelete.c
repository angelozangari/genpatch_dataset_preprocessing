 *****************************************************************************/
void sout_MuxDelete( sout_mux_t *p_mux )
{
    if( p_mux->p_module )
    {
        module_unneed( p_mux, p_mux->p_module );
    }
    free( p_mux->psz_mux );
    config_ChainDestroy( p_mux->p_cfg );
    vlc_object_release( p_mux );
}

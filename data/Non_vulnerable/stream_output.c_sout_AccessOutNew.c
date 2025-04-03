 *****************************************************************************/
sout_access_out_t *sout_AccessOutNew( vlc_object_t *p_sout,
                                      const char *psz_access, const char *psz_name )
{
    sout_access_out_t *p_access;
    char              *psz_next;
    p_access = vlc_custom_create( p_sout, sizeof( *p_access ), "access out" );
    if( !p_access )
        return NULL;
    psz_next = config_ChainCreate( &p_access->psz_access, &p_access->p_cfg,
                                   psz_access );
    free( psz_next );
    p_access->psz_path   = strdup( psz_name ? psz_name : "" );
    p_access->p_sys      = NULL;
    p_access->pf_seek    = NULL;
    p_access->pf_read    = NULL;
    p_access->pf_write   = NULL;
    p_access->pf_control = NULL;
    p_access->p_module   = NULL;
    p_access->p_module   =
        module_need( p_access, "sout access", p_access->psz_access, true );
    if( !p_access->p_module )
    {
        free( p_access->psz_access );
        free( p_access->psz_path );
        vlc_object_release( p_access );
        return( NULL );
    }
    return p_access;
}

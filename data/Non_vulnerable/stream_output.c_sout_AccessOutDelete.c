 *****************************************************************************/
void sout_AccessOutDelete( sout_access_out_t *p_access )
{
    if( p_access->p_module )
    {
        module_unneed( p_access, p_access->p_module );
    }
    free( p_access->psz_access );
    config_ChainDestroy( p_access->p_cfg );
    free( p_access->psz_path );
    vlc_object_release( p_access );
}

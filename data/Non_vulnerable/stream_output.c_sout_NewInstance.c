 *****************************************************************************/
sout_instance_t *sout_NewInstance( vlc_object_t *p_parent, const char *psz_dest )
{
    sout_instance_t *p_sout;
    char *psz_chain;
    if( psz_dest && psz_dest[0] == '#' )
    {
        psz_chain = strdup( &psz_dest[1] );
    }
    else
    {
        psz_chain = sout_stream_url_to_chain(
            var_InheritBool(p_parent, "sout-display"), psz_dest );
    }
    if(!psz_chain)
        return NULL;
    /* *** Allocate descriptor *** */
    p_sout = vlc_custom_create( p_parent, sizeof( *p_sout ), "stream output" );
    if( p_sout == NULL )
    {
        free( psz_chain );
        return NULL;
    }
    msg_Dbg( p_sout, "using sout chain=`%s'", psz_chain );
    /* *** init descriptor *** */
    p_sout->psz_sout    = strdup( psz_dest );
    p_sout->i_out_pace_nocontrol = 0;
    vlc_mutex_init( &p_sout->lock );
    p_sout->p_stream = NULL;
    var_Create( p_sout, "sout-mux-caching", VLC_VAR_INTEGER | VLC_VAR_DOINHERIT );
    p_sout->p_stream = sout_StreamChainNew( p_sout, psz_chain, NULL, NULL );
    if( p_sout->p_stream )
    {
        free( psz_chain );
        return p_sout;
    }
    msg_Err( p_sout, "stream chain failed for `%s'", psz_chain );
    free( psz_chain );
    FREENULL( p_sout->psz_sout );
    vlc_mutex_destroy( &p_sout->lock );
    vlc_object_release( p_sout );
    return NULL;
}

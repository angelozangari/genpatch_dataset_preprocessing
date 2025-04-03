 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    sout_stream_t       *p_stream = (sout_stream_t*)p_this;
    sout_stream_sys_t   *p_sys;
    char *psz_mux, *psz_access, *psz_url;
    sout_access_out_t   *p_access;
    int                 ret = VLC_EGENERIC;
    config_ChainParse( p_stream, SOUT_CFG_PREFIX, ppsz_sout_options,
                   p_stream->p_cfg );
    psz_mux = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "mux" );
    psz_access = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "access" );
    if( !psz_access )
        psz_access = strdup(p_stream->psz_name);
    psz_url = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "dst" );
    if (!psz_url)
    {
        char *psz_bind = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "bind" );
        if( psz_bind )
        {
            char *psz_path = var_GetNonEmptyString( p_stream, SOUT_CFG_PREFIX "path" );
            if( psz_path )
            {
                if( asprintf( &psz_url, "%s/%s", psz_bind, psz_path ) == -1 )
                    psz_url = NULL;
                free(psz_bind);
                free( psz_path );
            }
            else
                psz_url = psz_bind;
        }
    }
    p_sys = p_stream->p_sys = malloc( sizeof( sout_stream_sys_t) );
    if( !p_sys )
    {
        ret = VLC_ENOMEM;
        goto end;
    }
    p_sys->p_session = NULL;
    if( fixAccessMux( p_stream, &psz_mux, &psz_access, psz_url ) )
        goto end;
    checkAccessMux( p_stream, psz_access, psz_mux );
    p_access = sout_AccessOutNew( p_stream, psz_access, psz_url );
    if( p_access == NULL )
    {
        msg_Err( p_stream, "no suitable sout access module for `%s/%s://%s'",
                 psz_access, psz_mux, psz_url );
        goto end;
    }
    p_sys->p_mux = sout_MuxNew( p_stream->p_sout, psz_mux, p_access );
    if( !p_sys->p_mux )
    {
        const char *psz_mux_guess = getMuxFromAlias( psz_mux );
        if( psz_mux_guess && strcmp( psz_mux_guess, psz_mux ) )
        {
            msg_Dbg( p_stream, "Couldn't open mux `%s', trying `%s' instead",
                psz_mux, psz_mux_guess );
            p_sys->p_mux = sout_MuxNew( p_stream->p_sout, psz_mux_guess, p_access );
        }
        if( !p_sys->p_mux )
        {
            msg_Err( p_stream, "no suitable sout mux module for `%s/%s://%s'",
                psz_access, psz_mux, psz_url );
            sout_AccessOutDelete( p_access );
            goto end;
        }
    }
    if( var_GetBool( p_stream, SOUT_CFG_PREFIX"sap" ) )
        create_SDP( p_stream, p_access );
    p_stream->pf_add    = Add;
    p_stream->pf_del    = Del;
    p_stream->pf_send   = Send;
    if( !sout_AccessOutCanControlPace( p_access ) )
        p_stream->pace_nocontrol = true;
    ret = VLC_SUCCESS;
    msg_Dbg( p_this, "using `%s/%s://%s'", psz_access, psz_mux, psz_url );
end:
    if( ret != VLC_SUCCESS )
        free( p_sys );
    free( psz_access );
    free( psz_mux );
    free( psz_url );
    return ret;
}

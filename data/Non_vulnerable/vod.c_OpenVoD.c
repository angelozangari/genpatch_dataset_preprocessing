 *****************************************************************************/
int OpenVoD( vlc_object_t *p_this )
{
    vod_t *p_vod = (vod_t *)p_this;
    vod_sys_t *p_sys = NULL;
    char *psz_url;
    p_vod->p_sys = p_sys = malloc( sizeof( vod_sys_t ) );
    if( !p_sys ) goto error;
    psz_url = var_InheritString( p_vod, "rtsp-host" );
    if( psz_url == NULL )
        p_sys->psz_rtsp_path = strdup( "/" );
    else
    {
        vlc_url_t url;
        vlc_UrlParse( &url, psz_url, 0 );
        free( psz_url );
        if( url.psz_path == NULL )
            p_sys->psz_rtsp_path = strdup( "/" );
        else
        if( !( strlen( url.psz_path ) > 0
               && url.psz_path[strlen( url.psz_path ) - 1] == '/' ) )
        {
            if( asprintf( &p_sys->psz_rtsp_path, "%s/", url.psz_path ) == -1 )
            {
                p_sys->psz_rtsp_path = NULL;
                vlc_UrlClean( &url );
                goto error;
            }
        }
        else
            p_sys->psz_rtsp_path = strdup( url.psz_path );
        vlc_UrlClean( &url );
    }
    p_vod->pf_media_new = MediaNew;
    p_vod->pf_media_del = MediaAskDel;
    p_sys->p_fifo_cmd = block_FifoNew();
    if( vlc_clone( &p_sys->thread, CommandThread, p_vod, VLC_THREAD_PRIORITY_LOW ) )
    {
        msg_Err( p_vod, "cannot spawn rtsp vod thread" );
        block_FifoRelease( p_sys->p_fifo_cmd );
        goto error;
    }
    return VLC_SUCCESS;
error:
    if( p_sys )
    {
        free( p_sys->psz_rtsp_path );
        free( p_sys );
    }
    return VLC_EGENERIC;
}

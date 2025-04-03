 *****************************************************************************/
static int Open( vlc_object_t *p_this )
{
    demux_t      *p_demux = (demux_t*)p_this;
    demux_sys_t  *p_sys;
    p_sys = calloc( 1, sizeof(demux_sys_t) );
    if( !p_sys ) return VLC_ENOMEM;
    p_sys->f_fps = var_InheritFloat( p_demux, CFG_PREFIX "fps" );
    if ( p_sys->f_fps <= 0 ) p_sys->f_fps = 1.0;
    p_sys->i_frame_interval = 1000000 / p_sys->f_fps ;
    char *psz_chroma = var_InheritString( p_demux, CFG_PREFIX "chroma" );
    vlc_fourcc_t i_chroma = vlc_fourcc_GetCodecFromString( VIDEO_ES, psz_chroma );
    free( psz_chroma );
    if ( !i_chroma || vlc_fourcc_IsYUV( i_chroma ) )
    {
        msg_Err( p_demux, "Only RGB chroma are supported" );
        free( p_sys );
        return VLC_EGENERIC;
    }
    const vlc_chroma_description_t *p_chroma_desc = vlc_fourcc_GetChromaDescription( i_chroma );
    if ( !p_chroma_desc )
    {
        msg_Err( p_demux, "Unable to get RGB chroma description" );
        free( p_sys );
        return VLC_EGENERIC;
    }
#ifdef NDEBUG
    rfbEnableClientLogging = FALSE;
#endif
    p_sys->p_client = rfbGetClient( p_chroma_desc->pixel_bits / 3, // bitsPerSample
                                    3, // samplesPerPixel
                                    p_chroma_desc->pixel_size ); // bytesPerPixel
    if ( ! p_sys->p_client )
    {
        msg_Dbg( p_demux, "Unable to set up client for %s",
                 vlc_fourcc_GetDescription( VIDEO_ES, i_chroma ) );
        free( p_sys );
        return VLC_EGENERIC;
    }
    msg_Dbg( p_demux, "set up client for %s %d %d %d",
             vlc_fourcc_GetDescription( VIDEO_ES, i_chroma ),
             p_chroma_desc->pixel_bits / 3, 3, p_chroma_desc->pixel_size );
    p_sys->p_client->MallocFrameBuffer = mallocFrameBufferHandler;
    p_sys->p_client->canHandleNewFBSize = TRUE;
    p_sys->p_client->GetCredential = getCredentialHandler;
    p_sys->p_client->GetPassword = getPasswordHandler; /* VNC simple auth */
    /* Set compression and quality levels */
    p_sys->p_client->appData.compressLevel =
            var_InheritInteger( p_demux, CFG_PREFIX "compress-level" );
    p_sys->p_client->appData.qualityLevel =
            var_InheritInteger( p_demux, CFG_PREFIX "quality-level" );
    /* Parse uri params */
    vlc_url_t url;
    vlc_UrlParse( &url, p_demux->psz_location, 0 );
    if ( !EMPTY_STR(url.psz_host) )
        p_sys->p_client->serverHost = strdup( url.psz_host );
    else
        p_sys->p_client->serverHost = strdup( "localhost" );
    p_sys->p_client->appData.viewOnly = TRUE;
    p_sys->p_client->serverPort = ( url.i_port > 0 ) ? url.i_port : 5900;
    msg_Dbg( p_demux, "VNC init %s host=%s port=%d",
             p_demux->psz_location,
             p_sys->p_client->serverHost,
             p_sys->p_client->serverPort );
    vlc_UrlClean( &url );
    /* make demux available for callback handlers */
    rfbClientSetClientData( p_sys->p_client, DemuxThread, p_demux );
    p_demux->p_sys = p_sys;
    if( !rfbInitClient( p_sys->p_client, NULL, NULL ) )
    {
        msg_Err( p_demux, "can't connect to RFB server" );
        goto error;
    }
    p_sys->i_starttime = mdate();
    if ( vlc_clone( &p_sys->thread, DemuxThread, p_demux, VLC_THREAD_PRIORITY_INPUT ) != VLC_SUCCESS )
    {
        msg_Err( p_demux, "can't spawn thread" );
        goto error;
    }
    p_demux->pf_demux = NULL;
    p_demux->pf_control = Control;
    return VLC_SUCCESS;
error:
    free( p_sys );
    return VLC_EGENERIC;
}

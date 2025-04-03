 *****************************************************************************/
static vod_media_t *MediaNew( vod_t *p_vod, const char *psz_name,
                              input_item_t *p_item )
{
    vod_media_t *p_media = calloc( 1, sizeof(vod_media_t) );
    if( !p_media )
        return NULL;
    p_media->p_vod = p_vod;
    p_media->rtsp = NULL;
    TAB_INIT( p_media->i_es, p_media->es );
    p_media->psz_mux = NULL;
    p_media->i_length = input_item_GetDuration( p_item );
    vlc_mutex_lock( &p_item->lock );
    msg_Dbg( p_vod, "media '%s' has %i declared ES", psz_name, p_item->i_es );
    for( int i = 0; i < p_item->i_es; i++ )
    {
        es_format_t *p_fmt = p_item->es[i];
        switch( p_fmt->i_codec )
        {
            case VLC_FOURCC( 'm', 'p', '2', 't' ):
                p_media->psz_mux = "ts";
                break;
            case VLC_FOURCC( 'm', 'p', '2', 'p' ):
                p_media->psz_mux = "ps";
                break;
        }
        assert(p_media->psz_mux == NULL || p_item->i_es == 1);
        media_es_t *p_es = calloc( 1, sizeof(media_es_t) );
        if( !p_es )
            continue;
        p_es->es_id = p_fmt->i_id;
        p_es->rtsp_id = NULL;
        if (rtp_get_fmt(VLC_OBJECT(p_vod), p_fmt, p_media->psz_mux,
                        &p_es->rtp_fmt) != VLC_SUCCESS)
        {
            free(p_es);
            continue;
        }
        TAB_APPEND( p_media->i_es, p_media->es, p_es );
        msg_Dbg(p_vod, "  - added ES %u %s (%4.4s)",
                p_es->rtp_fmt.payload_type, p_es->rtp_fmt.ptname,
                (char *)&p_fmt->i_codec);
    }
    vlc_mutex_unlock( &p_item->lock );
    if (p_media->i_es == 0)
    {
        msg_Err(p_vod, "no ES was added to the media, aborting");
        goto error;
    }
    msg_Dbg(p_vod, "adding media '%s'", psz_name);
    CommandPush( p_vod, RTSP_CMD_TYPE_ADD, p_media, psz_name );
    return p_media;
error:
    MediaDel(p_vod, p_media);
    return NULL;
}

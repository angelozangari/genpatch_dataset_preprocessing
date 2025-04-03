 *****************************************************************************/
static int vlm_MediaVodControl( void *p_private, vod_media_t *p_vod_media,
                                const char *psz_id, int i_query, va_list args )
{
    vlm_t *vlm = (vlm_t *)p_private;
    int i, i_ret;
    const char *psz;
    int64_t id;
    if( !p_private || !p_vod_media )
        return VLC_EGENERIC;
    vlc_mutex_lock( &vlm->lock );
    /* Find media id */
    for( i = 0, id = -1; i < vlm->i_media; i++ )
    {
        if( p_vod_media == vlm->media[i]->vod.p_media )
        {
            id = vlm->media[i]->cfg.id;
            break;
        }
    }
    if( id == -1 )
    {
        vlc_mutex_unlock( &vlm->lock );
        return VLC_EGENERIC;
    }
    switch( i_query )
    {
    case VOD_MEDIA_PLAY:
    {
        psz = (const char *)va_arg( args, const char * );
        int64_t *i_time = (int64_t *)va_arg( args, int64_t *);
        bool b_retry = false;
        if (*i_time < 0)
        {
            /* No start time requested: return the current NPT */
            i_ret = vlm_ControlInternal( vlm, VLM_GET_MEDIA_INSTANCE_TIME, id, psz_id, i_time );
            /* The instance is not running yet, it will start at 0 */
            if (i_ret)
                *i_time = 0;
        }
        else
        {
            /* We want to seek before unpausing, but it won't
             * work if the instance is not running yet. */
            b_retry = vlm_ControlInternal( vlm, VLM_SET_MEDIA_INSTANCE_TIME, id, psz_id, *i_time );
        }
        i_ret = vlm_ControlInternal( vlm, VLM_START_MEDIA_VOD_INSTANCE, id, psz_id, 0, psz );
        if (!i_ret && b_retry)
            i_ret = vlm_ControlInternal( vlm, VLM_SET_MEDIA_INSTANCE_TIME, id, psz_id, *i_time );
        break;
    }
    case VOD_MEDIA_PAUSE:
    {
        int64_t *i_time = (int64_t *)va_arg( args, int64_t *);
        i_ret = vlm_ControlInternal( vlm, VLM_PAUSE_MEDIA_INSTANCE, id, psz_id );
        if (!i_ret)
            i_ret = vlm_ControlInternal( vlm, VLM_GET_MEDIA_INSTANCE_TIME, id, psz_id, i_time );
        break;
    }
    case VOD_MEDIA_STOP:
        i_ret = vlm_ControlInternal( vlm, VLM_STOP_MEDIA_INSTANCE, id, psz_id );
        break;
    case VOD_MEDIA_SEEK:
    {
        int64_t i_time = (int64_t)va_arg( args, int64_t );
        i_ret = vlm_ControlInternal( vlm, VLM_SET_MEDIA_INSTANCE_TIME, id, psz_id, i_time );
        break;
    }
    case VOD_MEDIA_REWIND:
    {
        double d_scale = (double)va_arg( args, double );
        double d_position;
        vlm_ControlInternal( vlm, VLM_GET_MEDIA_INSTANCE_POSITION, id, psz_id, &d_position );
        d_position -= (d_scale / 1000.0);
        if( d_position < 0.0 )
            d_position = 0.0;
        i_ret = vlm_ControlInternal( vlm, VLM_SET_MEDIA_INSTANCE_POSITION, id, psz_id, d_position );
        break;
    }
    case VOD_MEDIA_FORWARD:
    {
        double d_scale = (double)va_arg( args, double );
        double d_position;
        vlm_ControlInternal( vlm, VLM_GET_MEDIA_INSTANCE_POSITION, id, psz_id, &d_position );
        d_position += (d_scale / 1000.0);
        if( d_position > 1.0 )
            d_position = 1.0;
        i_ret = vlm_ControlInternal( vlm, VLM_SET_MEDIA_INSTANCE_POSITION, id, psz_id, d_position );
        break;
    }
    default:
        i_ret = VLC_EGENERIC;
        break;
    }
    vlc_mutex_unlock( &vlm->lock );
    return i_ret;
}

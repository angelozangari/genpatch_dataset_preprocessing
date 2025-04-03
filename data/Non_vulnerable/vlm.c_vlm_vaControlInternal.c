}
static int vlm_vaControlInternal( vlm_t *p_vlm, int i_query, va_list args )
{
    vlm_media_t *p_dsc;
    vlm_media_t **pp_dsc;
    vlm_media_t ***ppp_dsc;
    vlm_media_instance_t ***ppp_idsc;
    const char *psz_id;
    const char *psz_vod;
    int64_t *p_id;
    int64_t id;
    int i_int;
    int *pi_int;
    int64_t *pi_i64;
    int64_t i_i64;
    double *pd_double;
    double d_double;
    switch( i_query )
    {
    /* Media control */
    case VLM_GET_MEDIAS:
        ppp_dsc = (vlm_media_t ***)va_arg( args, vlm_media_t *** );
        pi_int = (int *)va_arg( args, int * );
        return vlm_ControlMediaGets( p_vlm, ppp_dsc, pi_int );
    case VLM_CLEAR_MEDIAS:
        return vlm_ControlMediaClear( p_vlm );
    case VLM_CHANGE_MEDIA:
        p_dsc = (vlm_media_t*)va_arg( args, vlm_media_t * );
        return vlm_ControlMediaChange( p_vlm, p_dsc );
    case VLM_ADD_MEDIA:
        p_dsc = (vlm_media_t*)va_arg( args, vlm_media_t * );
        p_id = (int64_t*)va_arg( args, int64_t * );
        return vlm_ControlMediaAdd( p_vlm, p_dsc, p_id );
    case VLM_DEL_MEDIA:
        id = (int64_t)va_arg( args, int64_t );
        return vlm_ControlMediaDel( p_vlm, id );
    case VLM_GET_MEDIA:
        id = (int64_t)va_arg( args, int64_t );
        pp_dsc = (vlm_media_t **)va_arg( args, vlm_media_t ** );
        return vlm_ControlMediaGet( p_vlm, id, pp_dsc );
    case VLM_GET_MEDIA_ID:
        psz_id = (const char*)va_arg( args, const char * );
        p_id = (int64_t*)va_arg( args, int64_t * );
        return vlm_ControlMediaGetId( p_vlm, psz_id, p_id );
    /* Media instance control */
    case VLM_GET_MEDIA_INSTANCES:
        id = (int64_t)va_arg( args, int64_t );
        ppp_idsc = (vlm_media_instance_t ***)va_arg( args, vlm_media_instance_t *** );
        pi_int = (int *)va_arg( args, int *);
        return vlm_ControlMediaInstanceGets( p_vlm, id, ppp_idsc, pi_int );
    case VLM_CLEAR_MEDIA_INSTANCES:
        id = (int64_t)va_arg( args, int64_t );
        return vlm_ControlMediaInstanceClear( p_vlm, id );
    case VLM_START_MEDIA_BROADCAST_INSTANCE:
        id = (int64_t)va_arg( args, int64_t );
        psz_id = (const char*)va_arg( args, const char* );
        i_int = (int)va_arg( args, int );
        return vlm_ControlMediaInstanceStart( p_vlm, id, psz_id, i_int, NULL );
    case VLM_START_MEDIA_VOD_INSTANCE:
        id = (int64_t)va_arg( args, int64_t );
        psz_id = (const char*)va_arg( args, const char* );
        i_int = (int)va_arg( args, int );
        psz_vod = (const char*)va_arg( args, const char* );
        if( !psz_vod )
            return VLC_EGENERIC;
        return vlm_ControlMediaInstanceStart( p_vlm, id, psz_id, i_int, psz_vod );
    case VLM_STOP_MEDIA_INSTANCE:
        id = (int64_t)va_arg( args, int64_t );
        psz_id = (const char*)va_arg( args, const char* );
        return vlm_ControlMediaInstanceStop( p_vlm, id, psz_id );
    case VLM_PAUSE_MEDIA_INSTANCE:
        id = (int64_t)va_arg( args, int64_t );
        psz_id = (const char*)va_arg( args, const char* );
        return vlm_ControlMediaInstancePause( p_vlm, id, psz_id );
    case VLM_GET_MEDIA_INSTANCE_TIME:
        id = (int64_t)va_arg( args, int64_t );
        psz_id = (const char*)va_arg( args, const char* );
        pi_i64 = (int64_t*)va_arg( args, int64_t * );
        return vlm_ControlMediaInstanceGetTimePosition( p_vlm, id, psz_id, pi_i64, NULL );
    case VLM_GET_MEDIA_INSTANCE_POSITION:
        id = (int64_t)va_arg( args, int64_t );
        psz_id = (const char*)va_arg( args, const char* );
        pd_double = (double*)va_arg( args, double* );
        return vlm_ControlMediaInstanceGetTimePosition( p_vlm, id, psz_id, NULL, pd_double );
    case VLM_SET_MEDIA_INSTANCE_TIME:
        id = (int64_t)va_arg( args, int64_t );
        psz_id = (const char*)va_arg( args, const char* );
        i_i64 = (int64_t)va_arg( args, int64_t);
        return vlm_ControlMediaInstanceSetTimePosition( p_vlm, id, psz_id, i_i64, -1 );
    case VLM_SET_MEDIA_INSTANCE_POSITION:
        id = (int64_t)va_arg( args, int64_t );
        psz_id = (const char*)va_arg( args, const char* );
        d_double = (double)va_arg( args, double );
        return vlm_ControlMediaInstanceSetTimePosition( p_vlm, id, psz_id, -1, d_double );
    case VLM_CLEAR_SCHEDULES:
        return vlm_ControlScheduleClear( p_vlm );
    default:
        msg_Err( p_vlm, "unknown VLM query" );
        return VLC_EGENERIC;
    }
}

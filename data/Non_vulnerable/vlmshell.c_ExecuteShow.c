}
static int ExecuteShow( vlm_t *p_vlm, const char *psz_name, vlm_message_t **pp_status )
{
    vlm_media_sys_t *p_media;
    vlm_schedule_sys_t *p_schedule;
    if( !psz_name )
    {
        *pp_status = vlm_Show( p_vlm, NULL, NULL, NULL );
        return VLC_SUCCESS;
    }
    p_media = vlm_MediaSearch( p_vlm, psz_name );
    p_schedule = vlm_ScheduleSearch( p_vlm, psz_name );
    if( p_schedule != NULL )
        *pp_status = vlm_Show( p_vlm, NULL, p_schedule, NULL );
    else if( p_media != NULL )
        *pp_status = vlm_Show( p_vlm, p_media, NULL, NULL );
    else
        *pp_status = vlm_Show( p_vlm, NULL, NULL, psz_name );
    return VLC_SUCCESS;
}

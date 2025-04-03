}
static int ExecuteDel( vlm_t *p_vlm, const char *psz_name, vlm_message_t **pp_status )
{
    vlm_media_sys_t *p_media;
    vlm_schedule_sys_t *p_schedule;
    p_media = vlm_MediaSearch( p_vlm, psz_name );
    p_schedule = vlm_ScheduleSearch( p_vlm, psz_name );
    if( p_schedule != NULL )
    {
        vlm_ScheduleDelete( p_vlm, p_schedule );
    }
    else if( p_media != NULL )
    {
        vlm_ControlInternal( p_vlm, VLM_DEL_MEDIA, p_media->cfg.id );
    }
    else if( !strcmp(psz_name, "media") )
    {
        vlm_ControlInternal( p_vlm, VLM_CLEAR_MEDIAS );
    }
    else if( !strcmp(psz_name, "schedule") )
    {
        vlm_ControlInternal( p_vlm, VLM_CLEAR_SCHEDULES );
    }
    else if( !strcmp(psz_name, "all") )
    {
        vlm_ControlInternal( p_vlm, VLM_CLEAR_MEDIAS );
        vlm_ControlInternal( p_vlm, VLM_CLEAR_SCHEDULES );
    }
    else
    {
        *pp_status = vlm_MessageNew( "del", "%s: media unknown", psz_name );
        return VLC_EGENERIC;
    }
    *pp_status = vlm_MessageSimpleNew( "del" );
    return VLC_SUCCESS;
}

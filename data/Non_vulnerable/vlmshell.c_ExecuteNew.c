}
static int ExecuteNew( vlm_t *p_vlm, const char *psz_name, const char *psz_type, const int i_property, char *ppsz_property[], vlm_message_t **pp_status )
{
    /* Check name */
    if( !strcmp( psz_name, "all" ) || !strcmp( psz_name, "media" ) || !strcmp( psz_name, "schedule" ) )
    {
        *pp_status = vlm_MessageNew( "new", "\"all\", \"media\" and \"schedule\" are reserved names" );
        return VLC_EGENERIC;
    }
    if( ExecuteIsMedia( p_vlm, psz_name ) || ExecuteIsSchedule( p_vlm, psz_name ) )
    {
        *pp_status = vlm_MessageNew( "new", "%s: Name already in use", psz_name );
        return VLC_EGENERIC;
    }
    /* */
    if( !strcmp( psz_type, "schedule" ) )
    {
        vlm_schedule_sys_t *p_schedule = vlm_ScheduleNew( p_vlm, psz_name );
        if( !p_schedule )
        {
            *pp_status = vlm_MessageNew( "new", "could not create schedule" );
            return VLC_EGENERIC;
        }
        return ExecuteScheduleProperty( p_vlm, p_schedule, true, i_property, ppsz_property, pp_status );
    }
    else if( !strcmp( psz_type, "vod" ) || !strcmp( psz_type, "broadcast" ) )
    {
        vlm_media_t cfg;
        int64_t id;
        vlm_media_Init( &cfg );
        cfg.psz_name = strdup( psz_name );
        cfg.b_vod = !strcmp( psz_type, "vod" );
        if( vlm_ControlInternal( p_vlm, VLM_ADD_MEDIA, &cfg, &id ) )
        {
            vlm_media_Clean( &cfg );
            *pp_status = vlm_MessageNew( "new", "could not create media" );
            return VLC_EGENERIC;
        }
        vlm_media_Clean( &cfg );
        return ExecuteMediaProperty( p_vlm, id, true, i_property, ppsz_property, pp_status );
    }
    else
    {
        *pp_status = vlm_MessageNew( "new", "%s: Choose between vod, broadcast or schedule", psz_type );
        return VLC_EGENERIC;
    }
}

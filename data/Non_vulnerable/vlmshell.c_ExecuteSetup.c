}
static int ExecuteSetup( vlm_t *p_vlm, const char *psz_name, const int i_property, char *ppsz_property[], vlm_message_t **pp_status )
{
    if( ExecuteIsSchedule( p_vlm, psz_name ) )
    {
        vlm_schedule_sys_t *p_schedule = vlm_ScheduleSearch( p_vlm, psz_name );
        return ExecuteScheduleProperty( p_vlm, p_schedule, false, i_property, ppsz_property, pp_status );
    }
    else if( ExecuteIsMedia( p_vlm, psz_name ) )
    {
        int64_t id;
        if( vlm_ControlInternal( p_vlm, VLM_GET_MEDIA_ID, psz_name, &id ) )
            goto error;
        return ExecuteMediaProperty( p_vlm, id, false, i_property, ppsz_property, pp_status );
    }
error:
    *pp_status = vlm_MessageNew( "setup", "%s unknown", psz_name );
    return VLC_EGENERIC;
}

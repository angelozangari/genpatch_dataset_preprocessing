}
static bool ExecuteIsSchedule( vlm_t *p_vlm, const char *psz_name )
{
    if( !psz_name || !vlm_ScheduleSearch( p_vlm, psz_name ) )
        return false;
    return true;
}

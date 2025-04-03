 *****************************************************************************/
static vlm_schedule_sys_t *vlm_ScheduleNew( vlm_t *vlm, const char *psz_name )
{
    if( !psz_name )
        return NULL;
    vlm_schedule_sys_t *p_sched = malloc( sizeof( vlm_schedule_sys_t ) );
    if( !p_sched )
        return NULL;
    p_sched->psz_name = strdup( psz_name );
    p_sched->b_enabled = false;
    p_sched->i_command = 0;
    p_sched->command = NULL;
    p_sched->i_date = 0;
    p_sched->i_period = 0;
    p_sched->i_repeat = -1;
    TAB_APPEND( vlm->i_schedule, vlm->schedule, p_sched );
    return p_sched;
}

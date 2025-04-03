}
static vlm_schedule_sys_t *vlm_ScheduleSearch( vlm_t *vlm, const char *psz_name )
{
    int i;
    for( i = 0; i < vlm->i_schedule; i++ )
    {
        if( strcmp( psz_name, vlm->schedule[i]->psz_name ) == 0 )
        {
            return vlm->schedule[i];
        }
    }
    return NULL;
}

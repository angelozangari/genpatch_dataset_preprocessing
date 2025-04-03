/* for now, simple delete. After, del with options (last arg) */
void vlm_ScheduleDelete( vlm_t *vlm, vlm_schedule_sys_t *sched )
{
    int i;
    if( sched == NULL ) return;
    TAB_REMOVE( vlm->i_schedule, vlm->schedule, sched );
    if( vlm->i_schedule == 0 ) free( vlm->schedule );
    free( sched->psz_name );
    for ( i = 0; i < sched->i_command; i++ )
        free( sched->command[i] );
    free( sched->command );
    free( sched );
}

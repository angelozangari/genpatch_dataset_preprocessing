 *****************************************************************************/
static void* Manage( void* p_object )
{
    vlm_t *vlm = (vlm_t*)p_object;
    int i, j;
    mtime_t i_lastcheck;
    mtime_t i_time;
    mtime_t i_nextschedule = 0;
    i_lastcheck = vlm_Date();
    for( ;; )
    {
        char **ppsz_scheduled_commands = NULL;
        int    i_scheduled_commands = 0;
        bool scheduled_command = false;
        vlc_mutex_lock( &vlm->lock_manage );
        mutex_cleanup_push( &vlm->lock_manage );
        while( !vlm->input_state_changed && !scheduled_command )
        {
            if( i_nextschedule )
                scheduled_command = vlc_cond_timedwait( &vlm->wait_manage, &vlm->lock_manage, i_nextschedule ) != 0;
            else
                vlc_cond_wait( &vlm->wait_manage, &vlm->lock_manage );
        }
        vlm->input_state_changed = false;
        vlc_cleanup_run( );
        int canc = vlc_savecancel ();
        /* destroy the inputs that wants to die, and launch the next input */
        vlc_mutex_lock( &vlm->lock );
        for( i = 0; i < vlm->i_media; i++ )
        {
            vlm_media_sys_t *p_media = vlm->media[i];
            for( j = 0; j < p_media->i_instance; )
            {
                vlm_media_instance_sys_t *p_instance = p_media->instance[j];
                if( p_instance->p_input && ( p_instance->p_input->b_eof || p_instance->p_input->b_error ) )
                {
                    int i_new_input_index;
                    /* */
                    i_new_input_index = p_instance->i_index + 1;
                    if( !p_media->cfg.b_vod && p_media->cfg.broadcast.b_loop && i_new_input_index >= p_media->cfg.i_input )
                        i_new_input_index = 0;
                    /* FIXME implement multiple input with VOD */
                    if( p_media->cfg.b_vod || i_new_input_index >= p_media->cfg.i_input )
                        vlm_ControlInternal( vlm, VLM_STOP_MEDIA_INSTANCE, p_media->cfg.id, p_instance->psz_name );
                    else
                        vlm_ControlInternal( vlm, VLM_START_MEDIA_BROADCAST_INSTANCE, p_media->cfg.id, p_instance->psz_name, i_new_input_index );
                    j = 0;
                }
                else
                {
                    j++;
                }
            }
        }
        /* scheduling */
        i_time = vlm_Date();
        i_nextschedule = 0;
        for( i = 0; i < vlm->i_schedule; i++ )
        {
            mtime_t i_real_date = vlm->schedule[i]->i_date;
            if( vlm->schedule[i]->b_enabled )
            {
                if( vlm->schedule[i]->i_date == 0 ) // now !
                {
                    vlm->schedule[i]->i_date = (i_time / 1000000) * 1000000 ;
                    i_real_date = i_time;
                }
                else if( vlm->schedule[i]->i_period != 0 )
                {
                    int j = 0;
                    while( vlm->schedule[i]->i_date + j *
                           vlm->schedule[i]->i_period <= i_lastcheck &&
                           ( vlm->schedule[i]->i_repeat > j ||
                             vlm->schedule[i]->i_repeat == -1 ) )
                    {
                        j++;
                    }
                    i_real_date = vlm->schedule[i]->i_date + j *
                        vlm->schedule[i]->i_period;
                }
                if( i_real_date <= i_time )
                {
                    if( i_real_date > i_lastcheck )
                    {
                        for( j = 0; j < vlm->schedule[i]->i_command; j++ )
                        {
                            TAB_APPEND( i_scheduled_commands,
                                        ppsz_scheduled_commands,
                                        strdup(vlm->schedule[i]->command[j] ) );
                        }
                    }
                }
                else if( i_nextschedule == 0 || i_real_date < i_nextschedule )
                {
                    i_nextschedule = i_real_date;
                }
            }
        }
        while( i_scheduled_commands )
        {
            vlm_message_t *message = NULL;
            char *psz_command = ppsz_scheduled_commands[0];
            ExecuteCommand( vlm, psz_command,&message );
            /* for now, drop the message */
            vlm_MessageDelete( message );
            TAB_REMOVE( i_scheduled_commands,
                        ppsz_scheduled_commands,
                        psz_command );
            free( psz_command );
        }
        i_lastcheck = i_time;
        vlc_mutex_unlock( &vlm->lock );
        vlc_restorecancel (canc);
    }
    return NULL;
}

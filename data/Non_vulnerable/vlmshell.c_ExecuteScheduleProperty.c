}
static int ExecuteScheduleProperty( vlm_t *p_vlm, vlm_schedule_sys_t *p_schedule, bool b_new,
                                    const int i_property, char *ppsz_property[], vlm_message_t **pp_status )
{
    const char *psz_cmd = b_new ? "new" : "setup";
    int i;
    for( i = 0; i < i_property; i++ )
    {
        if( !strcmp( ppsz_property[i], "enabled" ) ||
            !strcmp( ppsz_property[i], "disabled" ) )
        {
            if ( vlm_ScheduleSetup( p_schedule, ppsz_property[i], NULL ) )
                goto error;
        }
        else if( !strcmp( ppsz_property[i], "append" ) )
        {
            char *psz_line;
            int j;
            /* Beware: everything behind append is considered as
             * command line */
            if( ++i >= i_property )
                break;
            psz_line = strdup( ppsz_property[i] );
            for( j = i+1; j < i_property; j++ )
            {
                psz_line = xrealloc( psz_line,
                        strlen(psz_line) + strlen(ppsz_property[j]) + 1 + 1 );
                strcat( psz_line, " " );
                strcat( psz_line, ppsz_property[j] );
            }
            if( vlm_ScheduleSetup( p_schedule, "append", psz_line ) )
                goto error;
            break;
        }
        else
        {
            if( i + 1 >= i_property )
            {
                if( b_new )
                    vlm_ScheduleDelete( p_vlm, p_schedule );
                return ExecuteSyntaxError( psz_cmd, pp_status );
            }
            if( vlm_ScheduleSetup( p_schedule, ppsz_property[i], ppsz_property[i+1] ) )
                goto error;
            i++;
        }
    }
    *pp_status = vlm_MessageSimpleNew( psz_cmd );
    vlc_mutex_lock( &p_vlm->lock_manage );
    p_vlm->input_state_changed = true;
    vlc_cond_signal( &p_vlm->wait_manage );
    vlc_mutex_unlock( &p_vlm->lock_manage );
    return VLC_SUCCESS;
error:
    *pp_status = vlm_MessageNew( psz_cmd, "Error while setting the property '%s' to the schedule",
                                 ppsz_property[i] );
    return VLC_EGENERIC;
}

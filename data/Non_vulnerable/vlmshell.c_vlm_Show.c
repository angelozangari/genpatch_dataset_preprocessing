}
static vlm_message_t *vlm_Show( vlm_t *vlm, vlm_media_sys_t *media,
                                vlm_schedule_sys_t *schedule,
                                const char *psz_filter )
{
    if( media != NULL )
    {
        vlm_message_t *p_msg = vlm_MessageSimpleNew( "show" );
        if( p_msg )
            vlm_MessageAdd( p_msg, vlm_ShowMedia( media ) );
        return p_msg;
    }
    else if( schedule != NULL )
    {
        int i;
        vlm_message_t *msg;
        vlm_message_t *msg_schedule;
        vlm_message_t *msg_child;
        char buffer[100];
        msg = vlm_MessageSimpleNew( "show" );
        msg_schedule =
            vlm_MessageAdd( msg, vlm_MessageSimpleNew( schedule->psz_name ) );
        vlm_MessageAdd( msg_schedule, vlm_MessageNew("type", "schedule") );
        vlm_MessageAdd( msg_schedule,
                        vlm_MessageNew( "enabled", schedule->b_enabled ?
                                        "yes" : "no" ) );
        if( schedule->i_date != 0 )
        {
            struct tm date;
            time_t i_time = (time_t)( schedule->i_date / 1000000 );
            localtime_r( &i_time, &date);
            vlm_MessageAdd( msg_schedule,
                            vlm_MessageNew( "date", "%d/%d/%d-%d:%d:%d",
                                            date.tm_year + 1900, date.tm_mon + 1,
                                            date.tm_mday, date.tm_hour, date.tm_min,
                                            date.tm_sec ) );
        }
        else
            vlm_MessageAdd( msg_schedule, vlm_MessageNew("date", "now") );
        if( schedule->i_period != 0 )
        {
            time_t i_time = (time_t) ( schedule->i_period / 1000000 );
            struct tm date;
            date.tm_sec = (int)( i_time % 60 );
            i_time = i_time / 60;
            date.tm_min = (int)( i_time % 60 );
            i_time = i_time / 60;
            date.tm_hour = (int)( i_time % 24 );
            i_time = i_time / 24;
            date.tm_mday = (int)( i_time % 30 );
            i_time = i_time / 30;
            /* okay, okay, months are not always 30 days long */
            date.tm_mon = (int)( i_time % 12 );
            i_time = i_time / 12;
            date.tm_year = (int)i_time;
            sprintf( buffer, "%d/%d/%d-%d:%d:%d", date.tm_year, date.tm_mon,
                     date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
            vlm_MessageAdd( msg_schedule, vlm_MessageNew("period", "%s", buffer) );
        }
        else
            vlm_MessageAdd( msg_schedule, vlm_MessageNew("period", "0") );
        sprintf( buffer, "%d", schedule->i_repeat );
        vlm_MessageAdd( msg_schedule, vlm_MessageNew( "repeat", "%s", buffer ) );
        msg_child =
            vlm_MessageAdd( msg_schedule, vlm_MessageSimpleNew("commands" ) );
        for( i = 0; i < schedule->i_command; i++ )
        {
           vlm_MessageAdd( msg_child,
                           vlm_MessageSimpleNew( schedule->command[i] ) );
        }
        return msg;
    }
    else if( psz_filter && !strcmp( psz_filter, "media" ) )
    {
        vlm_message_t *p_msg;
        vlm_message_t *p_msg_child;
        int i_vod = 0, i_broadcast = 0;
        for( int i = 0; i < vlm->i_media; i++ )
        {
            if( vlm->media[i]->cfg.b_vod )
                i_vod++;
            else
                i_broadcast++;
        }
        p_msg = vlm_MessageSimpleNew( "show" );
        p_msg_child = vlm_MessageAdd( p_msg, vlm_MessageNew( "media",
                                      "( %d broadcast - %d vod )", i_broadcast,
                                      i_vod ) );
        for( int i = 0; i < vlm->i_media; i++ )
            vlm_MessageAdd( p_msg_child, vlm_ShowMedia( vlm->media[i] ) );
        return p_msg;
    }
    else if( psz_filter && !strcmp( psz_filter, "schedule" ) )
    {
        int i;
        vlm_message_t *msg;
        vlm_message_t *msg_child;
        msg = vlm_MessageSimpleNew( "show" );
        msg_child = vlm_MessageAdd( msg, vlm_MessageSimpleNew( "schedule" ) );
        for( i = 0; i < vlm->i_schedule; i++ )
        {
            vlm_schedule_sys_t *s = vlm->schedule[i];
            vlm_message_t *msg_schedule;
            mtime_t i_time, i_next_date;
            msg_schedule = vlm_MessageAdd( msg_child,
                                           vlm_MessageSimpleNew( s->psz_name ) );
            vlm_MessageAdd( msg_schedule,
                            vlm_MessageNew( "enabled", s->b_enabled ?
                                            "yes" : "no" ) );
            /* calculate next date */
            i_time = vlm_Date();
            i_next_date = s->i_date;
            if( s->i_period != 0 )
            {
                int j = 0;
                while( s->i_date + j * s->i_period <= i_time &&
                       s->i_repeat > j )
                {
                    j++;
                }
                i_next_date = s->i_date + j * s->i_period;
            }
            if( i_next_date > i_time )
            {
                time_t i_date = (time_t) (i_next_date / 1000000) ;
                struct tm tm;
                char psz_date[32];
                strftime( psz_date, sizeof(psz_date), "%Y-%m-%d %H:%M:%S (%a)",
                          localtime_r( &i_date, &tm ) );
                vlm_MessageAdd( msg_schedule,
                                vlm_MessageNew( "next launch", "%s", psz_date ) );
            }
        }
        return msg;
    }
    else if( ( psz_filter == NULL ) && ( media == NULL ) && ( schedule == NULL ) )
    {
        vlm_message_t *show1 = vlm_Show( vlm, NULL, NULL, "media" );
        vlm_message_t *show2 = vlm_Show( vlm, NULL, NULL, "schedule" );
        vlm_MessageAdd( show1, show2->child[0] );
        /* We must destroy the parent node "show" of show2
         * and not the children */
        free( show2->psz_name );
        free( show2 );
        return show1;
    }
    else
    {
        return vlm_MessageSimpleNew( "show" );
    }
}

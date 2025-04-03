}
static char *Save( vlm_t *vlm )
{
    char *save = NULL;
    char psz_header[] = "\n"
                        "# VLC media player VLM command batch\n"
                        "# http://www.videolan.org/vlc/\n\n" ;
    char *p;
    int i,j;
    int i_length = strlen( psz_header );
    for( i = 0; i < vlm->i_media; i++ )
    {
        vlm_media_sys_t *media = vlm->media[i];
        vlm_media_t *p_cfg = &media->cfg;
        if( p_cfg->b_vod )
            i_length += strlen( "new * vod " ) + strlen(p_cfg->psz_name);
        else
            i_length += strlen( "new * broadcast " ) + strlen(p_cfg->psz_name);
        if( p_cfg->b_enabled )
            i_length += strlen( "enabled" );
        else
            i_length += strlen( "disabled" );
        if( !p_cfg->b_vod && p_cfg->broadcast.b_loop )
            i_length += strlen( " loop\n" );
        else
            i_length += strlen( "\n" );
        for( j = 0; j < p_cfg->i_input; j++ )
            i_length += strlen( "setup * input \"\"\n" ) + strlen( p_cfg->psz_name ) + strlen( p_cfg->ppsz_input[j] );
        if( p_cfg->psz_output != NULL )
            i_length += strlen( "setup * output \n" ) + strlen(p_cfg->psz_name) + strlen(p_cfg->psz_output);
        for( j = 0; j < p_cfg->i_option; j++ )
            i_length += strlen("setup * option \n") + strlen(p_cfg->psz_name) + strlen(p_cfg->ppsz_option[j]);
        if( p_cfg->b_vod && p_cfg->vod.psz_mux )
            i_length += strlen("setup * mux \n") + strlen(p_cfg->psz_name) + strlen(p_cfg->vod.psz_mux);
    }
    for( i = 0; i < vlm->i_schedule; i++ )
    {
        vlm_schedule_sys_t *schedule = vlm->schedule[i];
        i_length += strlen( "new  schedule " ) + strlen( schedule->psz_name );
        if( schedule->b_enabled )
        {
            i_length += strlen( "date //-:: enabled\n" ) + 14;
        }
        else
        {
            i_length += strlen( "date //-:: disabled\n" ) + 14;
        }
        if( schedule->i_period != 0 )
        {
            i_length += strlen( "setup  " ) + strlen( schedule->psz_name ) +
                strlen( "period //-::\n" ) + 14;
        }
        if( schedule->i_repeat >= 0 )
        {
            char buffer[12];
            sprintf( buffer, "%d", schedule->i_repeat );
            i_length += strlen( "setup  repeat \n" ) +
                strlen( schedule->psz_name ) + strlen( buffer );
        }
        else
        {
            i_length++;
        }
        for( j = 0; j < schedule->i_command; j++ )
        {
            i_length += strlen( "setup  append \n" ) +
                strlen( schedule->psz_name ) + strlen( schedule->command[j] );
        }
    }
    /* Don't forget the '\0' */
    i_length++;
    /* now we have the length of save */
    p = save = malloc( i_length );
    if( !save ) return NULL;
    *save = '\0';
    p += sprintf( p, "%s", psz_header );
    /* finally we can write in it */
    for( i = 0; i < vlm->i_media; i++ )
    {
        vlm_media_sys_t *media = vlm->media[i];
        vlm_media_t *p_cfg = &media->cfg;
        if( p_cfg->b_vod )
            p += sprintf( p, "new %s vod ", p_cfg->psz_name );
        else
            p += sprintf( p, "new %s broadcast ", p_cfg->psz_name );
        if( p_cfg->b_enabled )
            p += sprintf( p, "enabled" );
        else
            p += sprintf( p, "disabled" );
        if( !p_cfg->b_vod && p_cfg->broadcast.b_loop )
            p += sprintf( p, " loop\n" );
        else
            p += sprintf( p, "\n" );
        for( j = 0; j < p_cfg->i_input; j++ )
            p += sprintf( p, "setup %s input \"%s\"\n", p_cfg->psz_name, p_cfg->ppsz_input[j] );
        if( p_cfg->psz_output )
            p += sprintf( p, "setup %s output %s\n", p_cfg->psz_name, p_cfg->psz_output );
        for( j = 0; j < p_cfg->i_option; j++ )
            p += sprintf( p, "setup %s option %s\n", p_cfg->psz_name, p_cfg->ppsz_option[j] );
        if( p_cfg->b_vod && p_cfg->vod.psz_mux )
            p += sprintf( p, "setup %s mux %s\n", p_cfg->psz_name, p_cfg->vod.psz_mux );
    }
    /* and now, the schedule scripts */
    for( i = 0; i < vlm->i_schedule; i++ )
    {
        vlm_schedule_sys_t *schedule = vlm->schedule[i];
        struct tm date;
        time_t i_time = (time_t) ( schedule->i_date / 1000000 );
        localtime_r( &i_time, &date);
        p += sprintf( p, "new %s schedule ", schedule->psz_name);
        if( schedule->b_enabled )
        {
            p += sprintf( p, "date %d/%d/%d-%d:%d:%d enabled\n",
                          date.tm_year + 1900, date.tm_mon + 1, date.tm_mday,
                          date.tm_hour, date.tm_min, date.tm_sec );
        }
        else
        {
            p += sprintf( p, "date %d/%d/%d-%d:%d:%d disabled\n",
                          date.tm_year + 1900, date.tm_mon + 1, date.tm_mday,
                          date.tm_hour, date.tm_min, date.tm_sec);
        }
        if( schedule->i_period != 0 )
        {
            p += sprintf( p, "setup %s ", schedule->psz_name );
            i_time = (time_t) ( schedule->i_period / 1000000 );
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
            p += sprintf( p, "period %d/%d/%d-%d:%d:%d\n",
                          date.tm_year, date.tm_mon, date.tm_mday,
                          date.tm_hour, date.tm_min, date.tm_sec);
        }
        if( schedule->i_repeat >= 0 )
        {
            p += sprintf( p, "setup %s repeat %d\n",
                          schedule->psz_name, schedule->i_repeat );
        }
        else
        {
            p += sprintf( p, "\n" );
        }
        for( j = 0; j < schedule->i_command; j++ )
        {
            p += sprintf( p, "setup %s append %s\n",
                          schedule->psz_name, schedule->command[j] );
        }
    }
    return save;
}

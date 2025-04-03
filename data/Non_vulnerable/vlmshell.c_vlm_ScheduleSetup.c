/* Ok, setup schedule command will be able to support only one (argument value) at a time  */
static int vlm_ScheduleSetup( vlm_schedule_sys_t *schedule, const char *psz_cmd,
                       const char *psz_value )
{
    if( !strcmp( psz_cmd, "enabled" ) )
    {
        schedule->b_enabled = true;
    }
    else if( !strcmp( psz_cmd, "disabled" ) )
    {
        schedule->b_enabled = false;
    }
    else if( !strcmp( psz_cmd, "date" ) )
    {
        struct tm time;
        const char *p;
        time_t date;
        time.tm_sec = 0;         /* seconds */
        time.tm_min = 0;         /* minutes */
        time.tm_hour = 0;        /* hours */
        time.tm_mday = 0;        /* day of the month */
        time.tm_mon = 0;         /* month */
        time.tm_year = 0;        /* year */
        time.tm_wday = 0;        /* day of the week */
        time.tm_yday = 0;        /* day in the year */
        time.tm_isdst = -1;       /* daylight saving time */
        /* date should be year/month/day-hour:minutes:seconds */
        p = strchr( psz_value, '-' );
        if( !strcmp( psz_value, "now" ) )
        {
            schedule->i_date = 0;
        }
        else if(p == NULL)
        {
            return 1;
        }
        else
        {
            unsigned i,j,k;
            switch( sscanf( p + 1, "%u:%u:%u", &i, &j, &k ) )
            {
                case 1:
                    time.tm_sec = i;
                    break;
                case 2:
                    time.tm_min = i;
                    time.tm_sec = j;
                    break;
                case 3:
                    time.tm_hour = i;
                    time.tm_min = j;
                    time.tm_sec = k;
                    break;
                default:
                    return 1;
            }
            switch( sscanf( psz_value, "%d/%d/%d", &i, &j, &k ) )
            {
                case 1:
                    time.tm_mday = i;
                    break;
                case 2:
                    time.tm_mon = i - 1;
                    time.tm_mday = j;
                    break;
                case 3:
                    time.tm_year = i - 1900;
                    time.tm_mon = j - 1;
                    time.tm_mday = k;
                    break;
                default:
                    return 1;
            }
            date = mktime( &time );
            schedule->i_date = ((mtime_t) date) * 1000000;
        }
    }
    else if( !strcmp( psz_cmd, "period" ) )
    {
        struct tm time;
        const char *p;
        const char *psz_time = NULL, *psz_date = NULL;
        time_t date;
        unsigned i,j,k;
        /* First, if date or period are modified, repeat should be equal to -1 */
        schedule->i_repeat = -1;
        time.tm_sec = 0;         /* seconds */
        time.tm_min = 0;         /* minutes */
        time.tm_hour = 0;        /* hours */
        time.tm_mday = 0;        /* day of the month */
        time.tm_mon = 0;         /* month */
        time.tm_year = 0;        /* year */
        time.tm_wday = 0;        /* day of the week */
        time.tm_yday = 0;        /* day in the year */
        time.tm_isdst = -1;       /* daylight saving time */
        /* date should be year/month/day-hour:minutes:seconds */
        p = strchr( psz_value, '-' );
        if( p )
        {
            psz_date = psz_value;
            psz_time = p + 1;
        }
        else
        {
            psz_time = psz_value;
        }
        switch( sscanf( psz_time, "%u:%u:%u", &i, &j, &k ) )
        {
            case 1:
                time.tm_sec = i;
                break;
            case 2:
                time.tm_min = i;
                time.tm_sec = j;
                break;
            case 3:
                time.tm_hour = i;
                time.tm_min = j;
                time.tm_sec = k;
                break;
            default:
                return 1;
        }
        if( psz_date )
        {
            switch( sscanf( psz_date, "%u/%u/%u", &i, &j, &k ) )
            {
                case 1:
                    time.tm_mday = i;
                    break;
                case 2:
                    time.tm_mon = i;
                    time.tm_mday = j;
                    break;
                case 3:
                    time.tm_year = i;
                    time.tm_mon = j;
                    time.tm_mday = k;
                    break;
                default:
                    return 1;
            }
        }
        /* ok, that's stupid... who is going to schedule streams every 42 years ? */
        date = (((( time.tm_year * 12 + time.tm_mon ) * 30 + time.tm_mday ) * 24 + time.tm_hour ) * 60 + time.tm_min ) * 60 + time.tm_sec ;
        schedule->i_period = ((mtime_t) date) * 1000000;
    }
    else if( !strcmp( psz_cmd, "repeat" ) )
    {
        int i;
        if( sscanf( psz_value, "%d", &i ) == 1 )
        {
            schedule->i_repeat = i;
        }
        else
        {
            return 1;
        }
    }
    else if( !strcmp( psz_cmd, "append" ) )
    {
        char *command = strdup( psz_value );
        TAB_APPEND( schedule->i_command, schedule->command, command );
    }
    else
    {
        return 1;
    }
    return 0;
}

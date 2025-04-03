 */
time_t str_duration( const char *psz_duration )
{
    bool        timeDesignatorReached = false;
    time_t      res = 0;
    char*       end_ptr;
    if ( psz_duration == NULL )
        return -1;
    if ( ( *(psz_duration++) ) != 'P' )
        return -1;
    do
    {
        double number = strtod( psz_duration, &end_ptr );
        double      mul = 0;
        if ( psz_duration != end_ptr )
            psz_duration = end_ptr;
        switch( *psz_duration )
        {
            case 'M':
            {
                //M can mean month or minutes, if the 'T' flag has been reached.
                //We don't handle months though.
                if ( timeDesignatorReached == true )
                    mul = 60.0;
                break ;
            }
            case 'Y':
            case 'W':
                break ; //Don't handle this duration.
            case 'D':
                mul = 86400.0;
                break ;
            case 'T':
                timeDesignatorReached = true;
                break ;
            case 'H':
                mul = 3600.0;
                break ;
            case 'S':
                mul = 1.0;
                break ;
            default:
                break ;
        }
        res += (time_t)(mul * number);
        if ( *psz_duration )
            psz_duration++;
    } while ( *psz_duration );
    return res;
}

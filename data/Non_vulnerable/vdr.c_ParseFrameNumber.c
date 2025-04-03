 *****************************************************************************/
static int64_t ParseFrameNumber( const char *psz_line, float fps )
{
    unsigned h, m, s, f, n;
    /* hour:min:sec.frame (frame is optional) */
    n = sscanf( psz_line, "%u:%u:%u.%u", &h, &m, &s, &f );
    if( n >= 3 )
    {
        if( n < 4 )
            f = 1;
        int64_t i_seconds = (int64_t)h * 3600 + (int64_t)m * 60 + s;
        return (int64_t)( i_seconds * (double)fps ) + __MAX(1, f) - 1;
    }
    /* only a frame number */
    int64_t i_frame = strtoll( psz_line, NULL, 10 );
    return __MAX(1, i_frame) - 1;
}

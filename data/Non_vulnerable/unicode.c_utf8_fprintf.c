 */
int utf8_fprintf( FILE *stream, const char *fmt, ... )
{
    va_list ap;
    int res;
    va_start( ap, fmt );
    res = utf8_vfprintf( stream, fmt, ap );
    va_end( ap );
    return res;
}

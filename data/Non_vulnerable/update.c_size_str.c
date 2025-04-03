 */
static char *size_str( long int l_size )
{
    char *psz_tmp = NULL;
    int i_retval = 0;
    if( l_size >> 30 )
        i_retval = asprintf( &psz_tmp, _("%.1f GiB"), (float)l_size/(1<<30) );
    else if( l_size >> 20 )
        i_retval = asprintf( &psz_tmp, _("%.1f MiB"), (float)l_size/(1<<20) );
    else if( l_size >> 10 )
        i_retval = asprintf( &psz_tmp, _("%.1f KiB"), (float)l_size/(1<<10) );
    else
        i_retval = asprintf( &psz_tmp, _("%ld B"), l_size );
    return i_retval == -1 ? NULL : psz_tmp;
}

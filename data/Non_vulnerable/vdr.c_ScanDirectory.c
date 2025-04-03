 *****************************************************************************/
static bool ScanDirectory( access_t *p_access )
{
    access_sys_t *p_sys = p_access->p_sys;
    /* find first part and determine directory format */
    p_sys->b_ts_format = true;
    if( !ImportNextFile( p_access ) )
    {
        p_sys->b_ts_format = !p_sys->b_ts_format;
        if( !ImportNextFile( p_access ) )
            return false;
    }
    /* get all remaining parts */
    while( ImportNextFile( p_access ) )
        continue;
    /* import meta data etc. */
    ImportMeta( p_access );
    /* cut marks depend on meta data and file sizes */
    ImportMarks( p_access );
    return true;
}

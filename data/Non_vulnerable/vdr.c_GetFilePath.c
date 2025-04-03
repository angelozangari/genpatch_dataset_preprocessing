 *****************************************************************************/
static char *GetFilePath( access_t *p_access, unsigned i_file )
{
    char *psz_path;
    if( asprintf( &psz_path, p_access->p_sys->b_ts_format ?
        "%s" DIR_SEP "%05u.ts" : "%s" DIR_SEP "%03u.vdr",
        p_access->psz_filepath, i_file + 1 ) == -1 )
        return NULL;
    else
        return psz_path;
}

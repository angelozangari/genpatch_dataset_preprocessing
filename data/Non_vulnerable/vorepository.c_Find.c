}
static int Find( addons_finder_t *p_finder )
{
    bool b_done = false;
    while ( !b_done )
    {
        char *psz_uri = NULL;
        if ( ! asprintf( &psz_uri, ADDONS_REPO_SCHEMEHOST"/xml" ) ) return VLC_ENOMEM;
        b_done = true;
        stream_t *p_stream = stream_UrlNew( p_finder, psz_uri );
        free( psz_uri );
        if ( !p_stream ) return VLC_EGENERIC;
        if ( ! ParseCategoriesInfo( p_finder, p_stream ) )
        {
            /* no more entries have been read: was last page or error */
            b_done = true;
        }
        stream_Delete( p_stream );
    }
    return VLC_SUCCESS;
}

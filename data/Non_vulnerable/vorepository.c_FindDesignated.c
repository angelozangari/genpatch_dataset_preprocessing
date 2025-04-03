}
static int FindDesignated( addons_finder_t *p_finder )
{
    char *psz_manifest;
    const char *psz_path = p_finder->psz_uri + 7; // remove scheme
    if ( asprintf( &psz_manifest, "unzip://%s!/manifest.xml",
                   psz_path ) < 1 )
        return VLC_ENOMEM;
    stream_t *p_stream = stream_UrlNew( p_finder, psz_manifest );
    free( psz_manifest );
    if ( !p_stream ) return VLC_EGENERIC;
    if ( ParseCategoriesInfo( p_finder, p_stream ) )
    {
        /* Do archive uri fixup */
        FOREACH_ARRAY( addon_entry_t *p_entry, p_finder->entries )
        if ( likely( !p_entry->psz_archive_uri ) )
                p_entry->psz_archive_uri = strdup( p_finder->psz_uri );
        FOREACH_END()
    }
    else
    {
        stream_Delete( p_stream );
        return VLC_EGENERIC;
    }
    stream_Delete( p_stream );
    return VLC_SUCCESS;
}

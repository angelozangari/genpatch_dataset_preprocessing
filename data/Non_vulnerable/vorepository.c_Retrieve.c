}
static int Retrieve( addons_finder_t *p_finder, addon_entry_t *p_entry )
{
    vlc_mutex_lock( &p_entry->lock );
    if ( !p_entry->psz_archive_uri )
    {
        vlc_mutex_unlock( &p_entry->lock );
        return VLC_EGENERIC;
    }
    char *psz_archive_uri = strdup( p_entry->psz_archive_uri );
    vlc_mutex_unlock( &p_entry->lock );
    if ( !psz_archive_uri )
        return VLC_ENOMEM;
    /* get archive and parse manifest */
    stream_t *p_stream;
    if ( psz_archive_uri[0] == '/' )
    {
        /* Relative path */
        char *psz_uri;
        if ( ! asprintf( &psz_uri, ADDONS_REPO_SCHEMEHOST"%s", psz_archive_uri ) )
        {
            free( psz_archive_uri );
            return VLC_ENOMEM;
        }
        p_stream = stream_UrlNew( p_finder, psz_uri );
        free( psz_uri );
    }
    else
    {
        p_stream = stream_UrlNew( p_finder, psz_archive_uri );
    }
    msg_Dbg( p_finder, "downloading archive %s", psz_archive_uri );
    free ( psz_archive_uri );
    if ( !p_stream ) return VLC_EGENERIC;
    /* In case of pf_ reuse */
    if ( p_finder->p_sys->psz_tempfile )
    {
        vlc_unlink( p_finder->p_sys->psz_tempfile );
        FREENULL( p_finder->p_sys->psz_tempfile );
    }
    p_finder->p_sys->psz_tempfile = tempnam( NULL, "vlp" );
    if ( !p_finder->p_sys->psz_tempfile )
    {
        msg_Err( p_finder, "Can't create temp storage file" );
        stream_Delete( p_stream );
        return VLC_EGENERIC;
    }
    FILE *p_destfile = vlc_fopen( p_finder->p_sys->psz_tempfile, "w" );
    if( !p_destfile )
    {
        msg_Err( p_finder, "Failed to open addon temp storage file" );
        FREENULL(p_finder->p_sys->psz_tempfile);
        stream_Delete( p_stream );
        return VLC_EGENERIC;
    }
    char buffer[1<<10];
    int i_read = 0;
    while ( ( i_read = stream_Read( p_stream, &buffer, 1<<10 ) ) )
    {
        if ( fwrite( &buffer, i_read, 1, p_destfile ) < 1 )
        {
            msg_Err( p_finder, "Failed to write to Addon file" );
            fclose( p_destfile );
            stream_Delete( p_stream );
            return VLC_EGENERIC;
        }
    }
    fclose( p_destfile );
    stream_Delete( p_stream );
    msg_Dbg( p_finder, "Reading manifest from %s", p_finder->p_sys->psz_tempfile );
    char *psz_tempfileuri = vlc_path2uri( p_finder->p_sys->psz_tempfile, "unzip" );
    if ( !psz_tempfileuri )
        return VLC_ENOMEM;
    char *psz_manifest_uri;
    if ( asprintf( &psz_manifest_uri, "%s!/manifest.xml", psz_tempfileuri ) < 1 )
    {
        free( psz_tempfileuri );
        return VLC_ENOMEM;
    }
    p_stream = stream_UrlNew( p_finder, psz_manifest_uri );
    free( psz_manifest_uri );
    if ( !p_stream )
    {
        free( psz_tempfileuri );
        return VLC_EGENERIC;
    }
    vlc_mutex_lock( &p_entry->lock );
    int i_ret = ( ParseManifest( p_finder, p_entry, psz_tempfileuri, p_stream ) > 0 )
                    ? VLC_SUCCESS : VLC_EGENERIC;
    vlc_mutex_unlock( &p_entry->lock );
    free( psz_tempfileuri );
    stream_Delete( p_stream );
    return i_ret;
}

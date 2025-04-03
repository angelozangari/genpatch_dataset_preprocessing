}
static void* update_DownloadReal( void *obj )
{
    update_download_thread_t *p_udt = (update_download_thread_t *)obj;
    dialog_progress_bar_t *p_progress = NULL;
    long int l_size;
    long int l_downloaded = 0;
    float f_progress;
    char *psz_status;
    char *psz_downloaded = NULL;
    char *psz_size = NULL;
    char *psz_destfile = NULL;
    char *psz_tmpdestfile = NULL;
    FILE *p_file = NULL;
    stream_t *p_stream = NULL;
    void* p_buffer = NULL;
    int i_read;
    int canc;
    update_t *p_update = p_udt->p_update;
    char *psz_destdir = p_udt->psz_destdir;
    msg_Dbg( p_udt, "Opening Stream '%s'", p_update->release.psz_url );
    canc = vlc_savecancel ();
    /* Open the stream */
    p_stream = stream_UrlNew( p_udt, p_update->release.psz_url );
    if( !p_stream )
    {
        msg_Err( p_udt, "Failed to open %s for reading", p_update->release.psz_url );
        goto end;
    }
    /* Get the stream size */
    l_size = stream_Size( p_stream );
    /* Get the file name and open it*/
    psz_tmpdestfile = strrchr( p_update->release.psz_url, '/' );
    if( !psz_tmpdestfile )
    {
        msg_Err( p_udt, "The URL %s is badly formated",
                 p_update->release.psz_url );
        goto end;
    }
    psz_tmpdestfile++;
    if( asprintf( &psz_destfile, "%s%s", psz_destdir, psz_tmpdestfile ) == -1 )
        goto end;
    p_file = vlc_fopen( psz_destfile, "w" );
    if( !p_file )
    {
        msg_Err( p_udt, "Failed to open %s for writing", psz_destfile );
        dialog_FatalWait( p_udt, _("Saving file failed"),
            _("Failed to open \"%s\" for writing"),
             psz_destfile );
        goto end;
    }
    /* Create a buffer and fill it with the downloaded file */
    p_buffer = (void *)malloc( 1 << 10 );
    if( unlikely(p_buffer == NULL) )
        goto end;
    msg_Dbg( p_udt, "Downloading Stream '%s'", p_update->release.psz_url );
    psz_size = size_str( l_size );
    if( asprintf( &psz_status, _("%s\nDownloading... %s/%s %.1f%% done"),
        p_update->release.psz_url, "0.0", psz_size, 0.0 ) == -1 )
        goto end;
    p_progress = dialog_ProgressCreate( p_udt, _( "Downloading ..."),
                                        psz_status, _("Cancel") );
    free( psz_status );
    if( p_progress == NULL )
        goto end;
    while( !atomic_load( &p_udt->aborted ) &&
           ( i_read = stream_Read( p_stream, p_buffer, 1 << 10 ) ) &&
           !dialog_ProgressCancelled( p_progress ) )
    {
        if( fwrite( p_buffer, i_read, 1, p_file ) < 1 )
        {
            msg_Err( p_udt, "Failed to write into %s", psz_destfile );
            break;
        }
        l_downloaded += i_read;
        psz_downloaded = size_str( l_downloaded );
        f_progress = (float)l_downloaded/(float)l_size;
        if( asprintf( &psz_status, _( "%s\nDownloading... %s/%s - %.1f%% done" ),
                      p_update->release.psz_url, psz_downloaded, psz_size,
                      f_progress*100 ) != -1 )
        {
            dialog_ProgressSet( p_progress, psz_status, f_progress );
            free( psz_status );
        }
        free( psz_downloaded );
    }
    /* Finish the progress bar or delete the file if the user had canceled */
    fclose( p_file );
    p_file = NULL;
    if( !atomic_load( &p_udt->aborted ) &&
        !dialog_ProgressCancelled( p_progress ) )
    {
        dialog_ProgressDestroy( p_progress );
        p_progress = NULL;
    }
    else
    {
        vlc_unlink( psz_destfile );
        goto end;
    }
    signature_packet_t sign;
    if( download_signature( VLC_OBJECT( p_udt ), &sign,
            p_update->release.psz_url ) != VLC_SUCCESS )
    {
        vlc_unlink( psz_destfile );
        dialog_FatalWait( p_udt, _("File could not be verified"),
            _("It was not possible to download a cryptographic signature for "
              "the downloaded file \"%s\". Thus, it was deleted."),
            psz_destfile );
        msg_Err( p_udt, "Couldn't download signature of downloaded file" );
        goto end;
    }
    if( memcmp( sign.issuer_longid, p_update->p_pkey->longid, 8 ) )
    {
        vlc_unlink( psz_destfile );
        msg_Err( p_udt, "Invalid signature issuer" );
        dialog_FatalWait( p_udt, _("Invalid signature"),
            _("The cryptographic signature for the downloaded file \"%s\" was "
              "invalid and could not be used to securely verify it. Thus, the "
              "file was deleted."),
            psz_destfile );
        goto end;
    }
    if( sign.type != BINARY_SIGNATURE )
    {
        vlc_unlink( psz_destfile );
        msg_Err( p_udt, "Invalid signature type" );
        dialog_FatalWait( p_udt, _("Invalid signature"),
            _("The cryptographic signature for the downloaded file \"%s\" was "
              "invalid and could not be used to securely verify it. Thus, the "
              "file was deleted."),
            psz_destfile );
        goto end;
    }
    uint8_t *p_hash = hash_from_file( psz_destfile, &sign );
    if( !p_hash )
    {
        msg_Err( p_udt, "Unable to hash %s", psz_destfile );
        vlc_unlink( psz_destfile );
        dialog_FatalWait( p_udt, _("File not verifiable"),
            _("It was not possible to securely verify the downloaded file"
              " \"%s\". Thus, it was deleted."),
            psz_destfile );
        goto end;
    }
    if( p_hash[0] != sign.hash_verification[0] ||
        p_hash[1] != sign.hash_verification[1] )
    {
        vlc_unlink( psz_destfile );
        dialog_FatalWait( p_udt, _("File corrupted"),
            _("Downloaded file \"%s\" was corrupted. Thus, it was deleted."),
             psz_destfile );
        msg_Err( p_udt, "Bad hash for %s", psz_destfile );
        free( p_hash );
        goto end;
    }
    if( verify_signature( &sign, &p_update->p_pkey->key, p_hash )
            != VLC_SUCCESS )
    {
        vlc_unlink( psz_destfile );
        dialog_FatalWait( p_udt, _("File corrupted"),
            _("Downloaded file \"%s\" was corrupted. Thus, it was deleted."),
             psz_destfile );
        msg_Err( p_udt, "BAD SIGNATURE for %s", psz_destfile );
        free( p_hash );
        goto end;
    }
    msg_Info( p_udt, "%s authenticated", psz_destfile );
    free( p_hash );
#ifdef _WIN32
    int answer = dialog_Question( p_udt, _("Update VLC media player"),
    _("The new version was successfully downloaded. Do you want to close VLC and install it now?"),
    _("Install"), _("Cancel"), NULL);
    if(answer == 1)
    {
        wchar_t psz_wdestfile[MAX_PATH];
        MultiByteToWideChar( CP_UTF8, 0, psz_destfile, -1, psz_wdestfile, MAX_PATH );
        answer = (int)ShellExecuteW( NULL, L"open", psz_wdestfile, NULL, NULL, SW_SHOW);
        if(answer > 32)
            libvlc_Quit(p_udt->p_libvlc);
    }
#endif
end:
    if( p_progress )
        dialog_ProgressDestroy( p_progress );
    if( p_stream )
        stream_Delete( p_stream );
    if( p_file )
        fclose( p_file );
    free( psz_destdir );
    free( psz_destfile );
    free( p_buffer );
    free( psz_size );
    vlc_restorecancel( canc );
    return NULL;
}

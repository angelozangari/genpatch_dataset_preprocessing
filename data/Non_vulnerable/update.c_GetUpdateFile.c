 */
static bool GetUpdateFile( update_t *p_update )
{
    stream_t *p_stream = NULL;
    char *psz_version_line = NULL;
    char *psz_update_data = NULL;
    p_stream = stream_UrlNew( p_update->p_libvlc, UPDATE_VLC_STATUS_URL );
    if( !p_stream )
    {
        msg_Err( p_update->p_libvlc, "Failed to open %s for reading",
                 UPDATE_VLC_STATUS_URL );
        goto error;
    }
    const int64_t i_read = stream_Size( p_stream );
    if( i_read < 0 || i_read >= UINT16_MAX)
    {
        msg_Err(p_update->p_libvlc, "Status file too large");
        goto error;
    }
    psz_update_data = malloc( i_read + 1 ); /* terminating '\0' */
    if( !psz_update_data )
        goto error;
    if( stream_Read( p_stream, psz_update_data, i_read ) != i_read )
    {
        msg_Err( p_update->p_libvlc, "Couldn't download update file %s",
                UPDATE_VLC_STATUS_URL );
        goto error;
    }
    psz_update_data[i_read] = '\0';
    stream_Delete( p_stream );
    p_stream = NULL;
    /* first line : version number */
    char *psz_update_data_parser = psz_update_data;
    size_t i_len = strcspn( psz_update_data, "\r\n" );
    psz_update_data_parser += i_len;
    while( *psz_update_data_parser == '\r' || *psz_update_data_parser == '\n' )
        psz_update_data_parser++;
    if( !(psz_version_line = malloc( i_len + 1)) )
        goto error;
    strncpy( psz_version_line, psz_update_data, i_len );
    psz_version_line[i_len] = '\0';
    p_update->release.i_extra = 0;
    int ret = sscanf( psz_version_line, "%i.%i.%i.%i",
                    &p_update->release.i_major, &p_update->release.i_minor,
                    &p_update->release.i_revision, &p_update->release.i_extra);
    if( ret != 3 && ret != 4 )
    {
            msg_Err( p_update->p_libvlc, "Update version false formated" );
            goto error;
    }
    /* second line : URL */
    i_len = strcspn( psz_update_data_parser, "\r\n" );
    if( i_len == 0 )
    {
        msg_Err( p_update->p_libvlc, "Update file %s is corrupted: URL missing",
                 UPDATE_VLC_STATUS_URL );
        goto error;
    }
    if( !(p_update->release.psz_url = malloc( i_len + 1)) )
        goto error;
    strncpy( p_update->release.psz_url, psz_update_data_parser, i_len );
    p_update->release.psz_url[i_len] = '\0';
    psz_update_data_parser += i_len;
    while( *psz_update_data_parser == '\r' || *psz_update_data_parser == '\n' )
        psz_update_data_parser++;
    /* Remaining data : description */
    i_len = strlen( psz_update_data_parser );
    if( i_len == 0 )
    {
        msg_Err( p_update->p_libvlc,
                "Update file %s is corrupted: description missing",
                UPDATE_VLC_STATUS_URL );
        goto error;
    }
    if( !(p_update->release.psz_desc = malloc( i_len + 1)) )
        goto error;
    strncpy( p_update->release.psz_desc, psz_update_data_parser, i_len );
    p_update->release.psz_desc[i_len] = '\0';
    /* Now that we know the status is valid, we must download its signature
     * to authenticate it */
    signature_packet_t sign;
    if( download_signature( VLC_OBJECT( p_update->p_libvlc ), &sign,
            UPDATE_VLC_STATUS_URL ) != VLC_SUCCESS )
    {
        msg_Err( p_update->p_libvlc, "Couldn't download signature of status file" );
        goto error;
    }
    if( sign.type != BINARY_SIGNATURE && sign.type != TEXT_SIGNATURE )
    {
        msg_Err( p_update->p_libvlc, "Invalid signature type" );
        goto error;
    }
    p_update->p_pkey = (public_key_t*)malloc( sizeof( public_key_t ) );
    if( !p_update->p_pkey )
        goto error;
    if( parse_public_key( videolan_public_key, sizeof( videolan_public_key ),
                        p_update->p_pkey, NULL ) != VLC_SUCCESS )
    {
        msg_Err( p_update->p_libvlc, "Couldn't parse embedded public key, something went really wrong..." );
        FREENULL( p_update->p_pkey );
        goto error;
    }
    memcpy( p_update->p_pkey->longid, videolan_public_key_longid, 8 );
    if( memcmp( sign.issuer_longid, p_update->p_pkey->longid , 8 ) != 0 )
    {
        msg_Dbg( p_update->p_libvlc, "Need to download the GPG key" );
        public_key_t *p_new_pkey = download_key(
                VLC_OBJECT(p_update->p_libvlc),
                sign.issuer_longid, videolan_public_key_longid );
        if( !p_new_pkey )
        {
            msg_Err( p_update->p_libvlc, "Couldn't download GPG key" );
            FREENULL( p_update->p_pkey );
            goto error;
        }
        uint8_t *p_hash = hash_from_public_key( p_new_pkey );
        if( !p_hash )
        {
            msg_Err( p_update->p_libvlc, "Failed to hash signature" );
            free( p_new_pkey );
            FREENULL( p_update->p_pkey );
            goto error;
        }
        if( verify_signature( &p_new_pkey->sig,
                    &p_update->p_pkey->key, p_hash ) == VLC_SUCCESS )
        {
            free( p_hash );
            msg_Info( p_update->p_libvlc, "Key authenticated" );
            free( p_update->p_pkey );
            p_update->p_pkey = p_new_pkey;
        }
        else
        {
            free( p_hash );
            msg_Err( p_update->p_libvlc, "Key signature invalid !" );
            goto error;
        }
    }
    uint8_t *p_hash = hash_from_text( psz_update_data, &sign );
    if( !p_hash )
    {
        msg_Warn( p_update->p_libvlc, "Can't compute hash for status file" );
        goto error;
    }
    else if( p_hash[0] != sign.hash_verification[0] ||
        p_hash[1] != sign.hash_verification[1] )
    {
        msg_Warn( p_update->p_libvlc, "Bad hash for status file" );
        free( p_hash );
        goto error;
    }
    else if( verify_signature( &sign, &p_update->p_pkey->key, p_hash )
            != VLC_SUCCESS )
    {
        msg_Err( p_update->p_libvlc, "BAD SIGNATURE for status file" );
        free( p_hash );
        goto error;
    }
    else
    {
        msg_Info( p_update->p_libvlc, "Status file authenticated" );
        free( p_hash );
        free( psz_version_line );
        free( psz_update_data );
        return true;
    }
error:
    if( p_stream )
        stream_Delete( p_stream );
    free( psz_version_line );
    free( psz_update_data );
    return false;
}

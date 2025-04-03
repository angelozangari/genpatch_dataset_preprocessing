 */
int download_signature( vlc_object_t *p_this, signature_packet_t *p_sig,
                        const char *psz_url )
{
    char *psz_sig = (char*) malloc( strlen( psz_url ) + 4 + 1 ); /* ".asc" + \0 */
    if( !psz_sig )
        return VLC_ENOMEM;
    strcpy( psz_sig, psz_url );
    strcat( psz_sig, ".asc" );
    stream_t *p_stream = stream_UrlNew( p_this, psz_sig );
    free( psz_sig );
    if( !p_stream )
        return VLC_ENOMEM;
    int64_t i_size = stream_Size( p_stream );
    msg_Dbg( p_this, "Downloading signature (%"PRId64" bytes)", i_size );
    uint8_t *p_buf = (uint8_t*)malloc( i_size );
    if( !p_buf )
    {
        stream_Delete( p_stream );
        return VLC_ENOMEM;
    }
    int i_read = stream_Read( p_stream, p_buf, (int)i_size );
    stream_Delete( p_stream );
    if( i_read != (int)i_size )
    {
        msg_Dbg( p_this,
            "Couldn't download full signature (only %d bytes)", i_read );
        free( p_buf );
        return VLC_EGENERIC;
    }
    if( (uint8_t)*p_buf < 0x80 ) /* ASCII */
    {
        msg_Dbg( p_this, "Unarmoring signature" );
        uint8_t* p_unarmored = (uint8_t*) malloc( ( i_size * 3 ) / 4 + 1 );
        if( !p_unarmored )
        {
            free( p_buf );
            return VLC_EGENERIC;
        }
        int i_bytes = pgp_unarmor( (char*)p_buf, i_size, p_unarmored, i_size );
        free( p_buf );
        p_buf = p_unarmored;
        i_size = i_bytes;
        if( i_bytes < 2 )
        {
            free( p_buf );
            msg_Dbg( p_this, "Unarmoring failed : corrupted signature ?" );
            return VLC_EGENERIC;
        }
    }
    if( packet_type( *p_buf ) != SIGNATURE_PACKET )
    {
        msg_Dbg( p_this, "Not a signature: %d", *p_buf );
        free( p_buf );
        return VLC_EGENERIC;
    }
    size_t i_header_len = packet_header_len( *p_buf );
    if( ( i_header_len != 1 && i_header_len != 2 && i_header_len != 4 ) ||
        i_header_len + 1 > (size_t)i_size )
    {
        free( p_buf );
        msg_Dbg( p_this, "Invalid signature packet header" );
        return VLC_EGENERIC;
    }
    size_t i_len = scalar_number( p_buf+1, i_header_len );
    if( i_len + i_header_len + 1 != (size_t)i_size )
    {
        free( p_buf );
        msg_Dbg( p_this, "Invalid signature packet" );
        return VLC_EGENERIC;
    }
    int i_ret = parse_signature_packet( p_sig, p_buf+1+i_header_len, i_len );
    free( p_buf );
    if( i_ret != VLC_SUCCESS )
    {
        msg_Dbg( p_this, "Couldn't parse signature" );
        return i_ret;
    }
    if( p_sig->type != BINARY_SIGNATURE && p_sig->type != TEXT_SIGNATURE )
    {
        msg_Dbg( p_this, "Invalid signature type: %d", p_sig->type );
        if( p_sig->version == 4 )
        {
            free( p_sig->specific.v4.hashed_data );
            free( p_sig->specific.v4.unhashed_data );
        }
        return VLC_EGENERIC;
    }
    return VLC_SUCCESS;
}

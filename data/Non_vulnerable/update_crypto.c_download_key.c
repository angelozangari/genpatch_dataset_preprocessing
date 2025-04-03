 */
public_key_t *download_key( vlc_object_t *p_this,
                    const uint8_t *p_longid, const uint8_t *p_signature_issuer )
{
    char *psz_url;
    if( asprintf( &psz_url, "http://download.videolan.org/pub/keys/%.2X%.2X%.2X%.2X%.2X%.2X%.2X%.2X.asc",
                    p_longid[0], p_longid[1], p_longid[2], p_longid[3],
                    p_longid[4], p_longid[5], p_longid[6], p_longid[7] ) == -1 )
        return NULL;
    stream_t *p_stream = stream_UrlNew( p_this, psz_url );
    free( psz_url );
    if( !p_stream )
        return NULL;
    int64_t i_size = stream_Size( p_stream );
    if( i_size < 0 )
    {
        stream_Delete( p_stream );
        return NULL;
    }
    uint8_t *p_buf = (uint8_t*)malloc( i_size );
    if( !p_buf )
    {
        stream_Delete( p_stream );
        return NULL;
    }
    int i_read = stream_Read( p_stream, p_buf, (int)i_size );
    stream_Delete( p_stream );
    if( i_read != (int)i_size )
    {
        msg_Dbg( p_this, "Couldn't read full GPG key" );
        free( p_buf );
        return NULL;
    }
    public_key_t *p_pkey = (public_key_t*) malloc( sizeof( public_key_t ) );
    if( !p_pkey )
    {
        free( p_buf );
        return NULL;
    }
    memcpy( p_pkey->longid, p_longid, 8 );
    int i_error = parse_public_key( p_buf, i_read, p_pkey, p_signature_issuer );
    free( p_buf );
    if( i_error != VLC_SUCCESS )
    {
        msg_Dbg( p_this, "Couldn't parse GPG key" );
        free( p_pkey );
        return NULL;
    }
    return p_pkey;
}

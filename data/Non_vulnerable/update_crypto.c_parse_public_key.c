 */
int parse_public_key( const uint8_t *p_key_data, size_t i_key_len,
                      public_key_t *p_key, const uint8_t *p_sig_issuer )
{
    const uint8_t *pos = p_key_data;
    const uint8_t *max_pos = pos + i_key_len;
    int i_status = 0;
#define PUBLIC_KEY_FOUND    0x01
#define USER_ID_FOUND       0x02
#define SIGNATURE_FOUND     0X04
    uint8_t *p_key_unarmored = NULL;
    p_key->psz_username = NULL;
    p_key->sig.specific.v4.hashed_data = NULL;
    p_key->sig.specific.v4.unhashed_data = NULL;
    if( !( *pos & 0x80 ) )
    {   /* first byte is ASCII, unarmoring */
        p_key_unarmored = (uint8_t*)malloc( i_key_len );
        if( !p_key_unarmored )
            return VLC_ENOMEM;
        int i_len = pgp_unarmor( (char*)p_key_data, i_key_len,
                                 p_key_unarmored, i_key_len );
        if( i_len == 0 )
            goto error;
        pos = p_key_unarmored;
        max_pos = pos + i_len;
    }
    while( pos < max_pos )
    {
        if( !(*pos & 0x80) || *pos & 0x40 )
            goto error;
        int i_type = packet_type( *pos );
        int i_header_len = packet_header_len( *pos++ );
        if( pos + i_header_len > max_pos ||
            ( i_header_len != 1 && i_header_len != 2 && i_header_len != 4 ) )
            goto error;
        int i_packet_len = scalar_number( pos, i_header_len );
        pos += i_header_len;
        if( pos + i_packet_len > max_pos )
            goto error;
        switch( i_type )
        {
            case PUBLIC_KEY_PACKET:
                i_status |= PUBLIC_KEY_FOUND;
                if( parse_public_key_packet( &p_key->key, pos, i_packet_len ) != VLC_SUCCESS )
                    goto error;
                break;
            case SIGNATURE_PACKET: /* we accept only v4 signatures here */
                if( i_status & SIGNATURE_FOUND || !p_sig_issuer )
                    break;
                int i_ret = parse_signature_packet( &p_key->sig, pos,
                                                    i_packet_len );
                if( i_ret == VLC_SUCCESS )
                {
                    if( p_key->sig.version != 4 )
                        break;
                    if( memcmp( p_key->sig.issuer_longid, p_sig_issuer, 8 ) )
                    {
                        free( p_key->sig.specific.v4.hashed_data );
                        free( p_key->sig.specific.v4.unhashed_data );
                        p_key->sig.specific.v4.hashed_data = NULL;
                        p_key->sig.specific.v4.unhashed_data = NULL;
                        break;
                    }
                    i_status |= SIGNATURE_FOUND;
                }
                break;
            case USER_ID_PACKET:
                if( p_key->psz_username ) /* save only the first User ID */
                    break;
                i_status |= USER_ID_FOUND;
                p_key->psz_username = (uint8_t*)malloc( i_packet_len + 1);
                if( !p_key->psz_username )
                    goto error;
                memcpy( p_key->psz_username, pos, i_packet_len );
                p_key->psz_username[i_packet_len] = '\0';
                break;
            default:
                break;
        }
        pos += i_packet_len;
    }
    free( p_key_unarmored );
    if( !( i_status & ( PUBLIC_KEY_FOUND | USER_ID_FOUND ) ) )
        return VLC_EGENERIC;
    if( p_sig_issuer && !( i_status & SIGNATURE_FOUND ) )
        return VLC_EGENERIC;
    return VLC_SUCCESS;
error:
    if( p_key->sig.version == 4 )
    {
        free( p_key->sig.specific.v4.hashed_data );
        free( p_key->sig.specific.v4.unhashed_data );
    }
    free( p_key->psz_username );
    free( p_key_unarmored );
    return VLC_EGENERIC;
}

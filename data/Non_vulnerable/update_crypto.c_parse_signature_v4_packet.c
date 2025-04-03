 */
static size_t parse_signature_v4_packet( signature_packet_t *p_sig,
                                      const uint8_t *p_buf, size_t i_sig_len )
{
    size_t i_read = 1; /* we already read the version byte */
    if( i_sig_len < 10 ) /* signature is at least 10 bytes + the 2 MPIs */
        return 0;
    p_sig->type = *p_buf++; i_read++;
    p_sig->public_key_algo = *p_buf++; i_read++;
    p_sig->digest_algo = *p_buf++; i_read++;
    memcpy( p_sig->specific.v4.hashed_data_len, p_buf, 2 );
    p_buf += 2; i_read += 2;
    size_t i_hashed_data_len =
        scalar_number( p_sig->specific.v4.hashed_data_len, 2 );
    i_read += i_hashed_data_len;
    if( i_read + 4 > i_sig_len )
        return 0;
    p_sig->specific.v4.hashed_data = (uint8_t*) malloc( i_hashed_data_len );
    if( !p_sig->specific.v4.hashed_data )
        return 0;
    memcpy( p_sig->specific.v4.hashed_data, p_buf, i_hashed_data_len );
    p_buf += i_hashed_data_len;
    memcpy( p_sig->specific.v4.unhashed_data_len, p_buf, 2 );
    p_buf += 2; i_read += 2;
    size_t i_unhashed_data_len =
        scalar_number( p_sig->specific.v4.unhashed_data_len, 2 );
    i_read += i_unhashed_data_len;
    if( i_read + 2 > i_sig_len )
        return 0;
    p_sig->specific.v4.unhashed_data = (uint8_t*) malloc( i_unhashed_data_len );
    if( !p_sig->specific.v4.unhashed_data )
        return 0;
    memcpy( p_sig->specific.v4.unhashed_data, p_buf, i_unhashed_data_len );
    p_buf += i_unhashed_data_len;
    memcpy( p_sig->hash_verification, p_buf, 2 );
    p_buf += 2; i_read += 2;
    uint8_t *p, *max_pos;
    p = p_sig->specific.v4.unhashed_data;
    max_pos = p + scalar_number( p_sig->specific.v4.unhashed_data_len, 2 );
    for( ;; )
    {
        if( p > max_pos )
            return 0;
        size_t i_subpacket_len;
        if( *p < 192 )
        {
            if( p + 1 > max_pos )
                return 0;
            i_subpacket_len = *p++;
        }
        else if( *p < 255 )
        {
            if( p + 2 > max_pos )
                return 0;
            i_subpacket_len = (*p++ - 192) << 8;
            i_subpacket_len += *p++ + 192;
        }
        else
        {
            if( p + 4 > max_pos )
                return 0;
            i_subpacket_len = *++p << 24;
            i_subpacket_len += *++p << 16;
            i_subpacket_len += *++p << 8;
            i_subpacket_len += *++p;
        }
        if( *p == ISSUER_SUBPACKET )
        {
            if( p + 9 > max_pos )
                return 0;
            memcpy( &p_sig->issuer_longid, p+1, 8 );
            return i_read;
        }
        p += i_subpacket_len;
    }
}

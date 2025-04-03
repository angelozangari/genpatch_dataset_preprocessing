}
static int parse_signature_packet( signature_packet_t *p_sig,
                                   const uint8_t *p_buf, size_t i_packet_len )
{
    if( !i_packet_len ) /* 1st sanity check, we need at least the version */
        return VLC_EGENERIC;
    p_sig->version = *p_buf++;
    size_t i_read;
    switch( p_sig->version )
    {
        case 3:
            i_read = parse_signature_v3_packet( p_sig, p_buf, i_packet_len );
            break;
        case 4:
            p_sig->specific.v4.hashed_data = NULL;
            p_sig->specific.v4.unhashed_data = NULL;
            i_read = parse_signature_v4_packet( p_sig, p_buf, i_packet_len );
            break;
        default:
            return VLC_EGENERIC;
    }
    if( i_read == 0 ) /* signature packet parsing has failed */
        goto error;
    if( p_sig->public_key_algo != GCRY_PK_DSA )
        goto error;
    switch( p_sig->type )
    {
        case BINARY_SIGNATURE:
        case TEXT_SIGNATURE:
        case GENERIC_KEY_SIGNATURE:
        case PERSONA_KEY_SIGNATURE:
        case CASUAL_KEY_SIGNATURE:
        case POSITIVE_KEY_SIGNATURE:
            break;
        default:
            goto error;
    }
    p_buf--; /* rewind to the version byte */
    p_buf += i_read;
    READ_MPI(p_sig->r, 256);
    READ_MPI(p_sig->s, 256);
    assert( i_read == i_packet_len );
    if( i_read < i_packet_len ) /* some extra data, hm ? */
        goto error;
    return VLC_SUCCESS;
error:
    if( p_sig->version == 4 )
    {
        free( p_sig->specific.v4.hashed_data );
        free( p_sig->specific.v4.unhashed_data );
    }
    return VLC_EGENERIC;
}

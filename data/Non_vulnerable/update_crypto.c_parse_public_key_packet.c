 */
static int parse_public_key_packet( public_key_packet_t *p_key,
                                    const uint8_t *p_buf, size_t i_packet_len )
{
    if( i_packet_len < 6 )
        return VLC_EGENERIC;
    size_t i_read = 0;
    p_key->version   = *p_buf++; i_read++;
    if( p_key->version != 4 )
        return VLC_EGENERIC;
    /* XXX: warn when timestamp is > date ? */
    memcpy( p_key->timestamp, p_buf, 4 ); p_buf += 4; i_read += 4;
    p_key->algo      = *p_buf++; i_read++;
    if( p_key->algo != GCRY_PK_DSA )
        return VLC_EGENERIC;
    READ_MPI(p_key->p, 3072);
    READ_MPI(p_key->q, 256);
    READ_MPI(p_key->g, 3072);
    READ_MPI(p_key->y, 3072);
    if( i_read == i_packet_len )
        return VLC_SUCCESS;
    /* some extra data eh ? */
error:
    return VLC_EGENERIC;
}

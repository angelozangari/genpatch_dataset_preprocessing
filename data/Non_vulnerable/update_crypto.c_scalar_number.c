#define packet_header_len( c ) ( ( c & 0x03 ) + 1 ) /* number of bytes in a packet header */
static inline int scalar_number( const uint8_t *p, int header_len )
{
    assert( header_len == 1 || header_len == 2 || header_len == 4 );
    if( header_len == 1 )
        return( p[0] );
    else if( header_len == 2 )
        return( (p[0] << 8) + p[1] );
    else if( header_len == 4 )
        return( (p[0] << 24) + (p[1] << 16) + (p[2] << 8) + p[3] );
    else
        abort();
}

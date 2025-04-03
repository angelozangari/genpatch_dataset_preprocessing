 */
uint8_t *hash_from_text( const char *psz_string,
        signature_packet_t *p_sig )
{
    gcry_md_hd_t hd;
    if( gcry_md_open( &hd, p_sig->digest_algo, 0 ) )
        return NULL;
    if( p_sig->type == TEXT_SIGNATURE )
    while( *psz_string )
    {
        size_t i_len = strcspn( psz_string, "\r\n" );
        if( i_len )
        {
            gcry_md_write( hd, psz_string, i_len );
            psz_string += i_len;
        }
        gcry_md_putc( hd, '\r' );
        gcry_md_putc( hd, '\n' );
        if( *psz_string == '\r' )
            psz_string++;
        if( *psz_string == '\n' )
            psz_string++;
    }
    else
        gcry_md_write( hd, psz_string, strlen( psz_string ) );
    return hash_finish( hd, p_sig );
}

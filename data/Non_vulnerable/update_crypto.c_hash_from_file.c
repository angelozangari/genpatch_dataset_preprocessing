 */
uint8_t *hash_from_file( const char *psz_file, signature_packet_t *p_sig )
{
    gcry_md_hd_t hd;
    if( gcry_md_open( &hd, p_sig->digest_algo, 0 ) )
        return NULL;
    if( hash_from_binary_file( psz_file, hd ) < 0 )
    {
        gcry_md_close( hd );
        return NULL;
    }
    return hash_finish( hd, p_sig );
}

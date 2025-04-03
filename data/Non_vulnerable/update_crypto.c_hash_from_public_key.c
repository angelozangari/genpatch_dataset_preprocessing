 */
uint8_t *hash_from_public_key( public_key_t *p_pkey )
{
    if( p_pkey->sig.version != 4 )
        return NULL;
    if( p_pkey->sig.type < GENERIC_KEY_SIGNATURE ||
        p_pkey->sig.type > POSITIVE_KEY_SIGNATURE )
        return NULL;
    gcry_error_t error = 0;
    gcry_md_hd_t hd;
    error = gcry_md_open( &hd, p_pkey->sig.digest_algo, 0 );
    if( error )
        return NULL;
    gcry_md_putc( hd, 0x99 );
    size_t i_p_len = mpi_len( p_pkey->key.p );
    size_t i_g_len = mpi_len( p_pkey->key.g );
    size_t i_q_len = mpi_len( p_pkey->key.q );
    size_t i_y_len = mpi_len( p_pkey->key.y );
    size_t i_size = 6 + 2*4 + i_p_len + i_g_len + i_q_len + i_y_len;
    gcry_md_putc( hd, (i_size >> 8) & 0xff );
    gcry_md_putc( hd, i_size & 0xff );
    gcry_md_putc( hd, p_pkey->key.version );
    gcry_md_write( hd, p_pkey->key.timestamp, 4 );
    gcry_md_putc( hd, p_pkey->key.algo );
    gcry_md_write( hd, (uint8_t*)&p_pkey->key.p, 2 );
    gcry_md_write( hd, (uint8_t*)&p_pkey->key.p + 2, i_p_len );
    gcry_md_write( hd, (uint8_t*)&p_pkey->key.q, 2 );
    gcry_md_write( hd, (uint8_t*)&p_pkey->key.q + 2, i_q_len );
    gcry_md_write( hd, (uint8_t*)&p_pkey->key.g, 2 );
    gcry_md_write( hd, (uint8_t*)&p_pkey->key.g + 2, i_g_len );
    gcry_md_write( hd, (uint8_t*)&p_pkey->key.y, 2 );
    gcry_md_write( hd, (uint8_t*)&p_pkey->key.y + 2, i_y_len );
    gcry_md_putc( hd, 0xb4 );
    size_t i_len = strlen((char*)p_pkey->psz_username);
    gcry_md_putc( hd, (i_len >> 24) & 0xff );
    gcry_md_putc( hd, (i_len >> 16) & 0xff );
    gcry_md_putc( hd, (i_len >> 8) & 0xff );
    gcry_md_putc( hd, (i_len) & 0xff );
    gcry_md_write( hd, p_pkey->psz_username, i_len );
    uint8_t *p_hash = hash_finish( hd, &p_pkey->sig );
    if( !p_hash ||
        p_hash[0] != p_pkey->sig.hash_verification[0] ||
        p_hash[1] != p_pkey->sig.hash_verification[1] )
    {
        free(p_hash);
        return NULL;
    }
    return p_hash;
}

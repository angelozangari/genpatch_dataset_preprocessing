/* final part of the hash */
static uint8_t *hash_finish( gcry_md_hd_t hd, signature_packet_t *p_sig )
{
    if( p_sig->version == 3 )
    {
        gcry_md_putc( hd, p_sig->type );
        gcry_md_write( hd, &p_sig->specific.v3.timestamp, 4 );
    }
    else if( p_sig->version == 4 )
    {
        gcry_md_putc( hd, p_sig->version );
        gcry_md_putc( hd, p_sig->type );
        gcry_md_putc( hd, p_sig->public_key_algo );
        gcry_md_putc( hd, p_sig->digest_algo );
        gcry_md_write( hd, p_sig->specific.v4.hashed_data_len, 2 );
        size_t i_len = scalar_number( p_sig->specific.v4.hashed_data_len, 2 );
        gcry_md_write( hd, p_sig->specific.v4.hashed_data, i_len );
        gcry_md_putc( hd, 0x04 );
        gcry_md_putc( hd, 0xFF );
        i_len += 6; /* hashed data + 6 bytes header */
        gcry_md_putc( hd, (i_len >> 24) & 0xff );
        gcry_md_putc( hd, (i_len >> 16) & 0xff );
        gcry_md_putc( hd, (i_len >> 8) & 0xff );
        gcry_md_putc( hd, (i_len) & 0xff );
    }
    else
    {   /* RFC 4880 only tells about versions 3 and 4 */
        return NULL;
    }
    gcry_md_final( hd );
    uint8_t *p_tmp = (uint8_t*) gcry_md_read( hd, p_sig->digest_algo) ;
    unsigned int hash_len = gcry_md_get_algo_dlen (p_sig->digest_algo);
    uint8_t *p_hash = malloc(hash_len);
    if( p_hash )
        memcpy(p_hash, p_tmp, hash_len);
    gcry_md_close( hd );
    return p_hash;
}
